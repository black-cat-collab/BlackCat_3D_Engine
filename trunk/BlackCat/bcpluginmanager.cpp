#include <QApplication>
#include <QPluginLoader>
#include <QFileDialog>
#include <QXmlStreamReader>
#include <QMessageBox>
#include <QMenu>
#include <QToolBar>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "BlackCat/bcpluginmanager.h"
#include "BlackCat/iwidgetplugin.h"
#include "BlackCat/isceneplugin.h"
#include "BlackCat/iloginplugin.h"
#include "BlackCat/imenuplugin.h"
#include "BlackCat/itoolplugin.h"
#include "BlackCat/idockplugin.h"
#include "BlackCat/iscenecontrolplugin.h"
#include "BlackCat/basicdefine.h"
#include "BlackCat/imainframeplugin.h"
#include "BlackCat/bcmessagebox.h"
#include "BlackCat/blackcat_global.h"
#include "proxy/servermanager.h"
#include "network/ihcnetwork.h"
#ifdef BCQT6
#include <QQuickWindow>
#else
#include <QtPlatformHeaders/QWindowsWindowFunctions>
#endif

#include <QMenu>
#include <QDialog>
#include <QProcess>
#include <QCoreApplication>
#ifdef __GNUC__
#include <sys/types.h>
#include <unistd.h>
#include <linux/input.h>
#define     _getpid()       getpid()
#define		sprintf_s       snprintf
#define     VK_F5           KEY_F5
#define     VK_F10          KEY_F10
#endif

static BCPluginManager* gThis = nullptr;

bool PluginCompFunc(IWidgetPlugin *pPlugin1, IWidgetPlugin *pPlugin2)
{
    return  (pPlugin2->iGetExecuteSort() > pPlugin1->iGetExecuteSort());
}

bool DockCompFunc(IDockPlugin *pDock1, IDockPlugin *pDock2)
{
    return  (pDock2->Index() >  pDock1->Index());
}

BCPluginManager::BCPluginManager() :
    m_pSystemAPI(nullptr),
    m_bPluginLoad(false),
    m_bFullScreen(true),
    m_bMousePress(false),
    m_pLoginPlugin(nullptr),
    m_pMainFramePlugin(nullptr),
    m_pUninitializeEngineFunc(nullptr),
    m_pBCWebView(nullptr),
    m_pAppSetting(nullptr),
    m_bCanKeyDown(true),
    m_nLastWindowWidth(1920),
    m_nLastWindowHeight(1080)
{
    gThis = this;
    qputenv("QTWEBENGINE_REMOTE_DEBUGGING", "5566");
#ifdef BCQT6
    qputenv("QTWEBENGINE_CHROMIUM_FLAGS", "--remote-allow-origins=*");
    //Qt6 不设置的话网页出不来会黑屏
    QQuickWindow::setGraphicsApi(QSGRendererInterface::OpenGL);
#endif

    //获取可用桌面大小
    QRect deskRect = QGuiApplication::primaryScreen()->geometry();
    m_WindowSize = deskRect.size();

    connect(this, SIGNAL(sigRunJsFromCore(QString)), this, SLOT(slotRunJSFromCore(QString)));
    connect(this, SIGNAL(sigFromWebFun(QJsonValue)),this,SLOT(slotFromWebFun(QJsonValue)),Qt::QueuedConnection);
    connect(this, SIGNAL(sigKillProcess(QString,QString)),this,SLOT(slotKillProcess(QString,QString)),Qt::QueuedConnection);

    //使用之前调用
    NetworkManager::initialize();
//    connect(NetworkManager::globalInstance(),&NetworkManager::downloadProgress,this,&BCPluginManager::downloadProgress);

//    QMTNetwork::RequestTask task;
//    task.eType = QMTNetwork::RequestType::Download;
//    task.url = QString("http://nodejs.org/dist/v16.17.0/node-v16.17.0-x64.msi");
//    task.strReqArg = QString("C:/Users/admin/Downloads");
//    task.bShowProgress = true;
//    task.bReplaceFileIfExist = true;
//    task.bTryAgainIfFailed = true;
//    task.nMaxRedirectionCount = 5;

//    NetworkReply *pReply = NetworkManager::globalInstance()->addRequest(task);
//    if (nullptr != pReply)
//    {
//        connect(pReply, &NetworkReply::requestFinished, this, &T::onRequestFinished);
//    }
}

//void BCPluginManager::downloadProgress(quint64 uiRequestId, qint64 iBytesDownload, qint64 iBytesTotal)
//{
//    qDebug()<<"iBtyes=" << iBytesDownload << ",iTotalBtyes=" << iBytesTotal;
//}

BCPluginManager::~BCPluginManager()
{
    if(m_pMainFramePlugin)
    {
        m_pMainFramePlugin->iShutDown();
    }

    if(m_pUninitializeEngineFunc)
    {
        qDebug()<<"release EngineDLL";
        m_pUninitializeEngineFunc();
        m_pSystemAPI = NULL;
    }

    //程序退出前调用
    NetworkManager::unInitialize();
}

bool BCPluginManager::iLogin()
{
    //    if(m_pLoginPlugin)
    //    {
    //        if(m_pLoginPlugin->Login())
    //        {
    //            return true;
    //        }
    //    }

    //    return false;

    iStartGame();
    return true;
}

void BCPluginManager::iSetPluginPrefix(QString prefix)
{
    m_strPrefix = prefix;
}

void BCPluginManager::iSetConfig(QString config)
{
    m_strConfig = config;
}

bool BCPluginManager::initEngine()
{
    do{
        qDebug() << "init engine";

#ifdef __GNUC__
        QLibrary kee3dLib("libEngine.so");
#else
        QLibrary kee3dLib("Engine.dll");
#endif

        //加载动态库
        kee3dLib.load();
        if (!kee3dLib.isLoaded())
        {
            BCMessageBox message(QStringLiteral("无法加载") + kee3dLib.fileName());
            message.addButton(QStringLiteral("确定"));
            message.exec();
            break;
        }
        func_InitializeEngine pInitEngineFunc = (func_InitializeEngine)kee3dLib.resolve("InitializeAPIEngine");
        if(!pInitEngineFunc)
        {
            BCMessageBox message(QStringLiteral("无法加载") + kee3dLib.fileName() + QStringLiteral("！") + kee3dLib.errorString());
            message.addButton(QStringLiteral("确定"));
            message.exec();

            return false;
        }

        m_pUninitializeEngineFunc = (func_UninitializeEngine)kee3dLib.resolve("UninitializeAPIEngine");

        QString strConfigName = m_strConfig;
        if(strConfigName.isEmpty())
        {
            strConfigName = m_strPrefix;
        }
        QString strOri = qAppName();
        QStringList listTemp = strOri.split("_");
        if(listTemp.length() >= 2)
        {
            QString str = strConfigName;
            str += "_" + listTemp[1];
            QFile file(QCoreApplication::applicationDirPath() + "/" + str + QString(".conf"));
            if(file.exists())
            {
                strConfigName = str;
            }

        }

        //strConfigName = "../../Data/Sky/Sky.conf";

        if(!strConfigName.contains(".conf"))
        {
            strConfigName = strConfigName + QString(".conf");
        }
        qDebug()<<"conf name="<<strConfigName;

        m_pSystemAPI = pInitEngineFunc(strConfigName.toLocal8Bit().toStdString());

        if(m_pSystemAPI == nullptr)
        {
            BCMessageBox message(strConfigName + QStringLiteral(", InitializeAPIEngine加载失败！"));
            message.addButton(QStringLiteral("确定"));
            message.exec();

            if(m_pUninitializeEngineFunc)
            {
                m_pUninitializeEngineFunc();
            }

            return false;
        }

        if(!loadProjectDll())
        {
            BCMessageBox message(QStringLiteral("无法加载项目动态库！"));
            message.addButton(QStringLiteral("确定"));
            message.exec();

            if(m_pUninitializeEngineFunc)
            {
                m_pUninitializeEngineFunc();
            }

            return false;
        }

        ServerManager::GetInstance()->SetCallback((fnCallBack)BCPluginManager::getFromWebSocket,ENUM_CB_RECV_BY_WEBSOCKET);
        if(m_pSystemAPI->iProjectClientAPI)
        {
            m_pSystemAPI->iProjectClientAPI->iSetExtraParam("BCPluginManager",(INT_PTR)this);
            m_pSystemAPI->iProjectClientAPI->iAddQtVsConnectFunc(static_forvs_connect_fn);
        }
        iPostInitEngine();
    }while(0);

    return true;
}

bool BCPluginManager::loadProjectDll()
{
    m_pSystemAPI->iCoreAPI->iRegistViewManager();

    return true;
}

void BCPluginManager::releaseProjectDll()
{

}

int BCPluginManager::iLoadPlugin()
{
    if(!initEngine())
    {
        return -1;
    }

    QString strApplicationName = QString::fromLocal8Bit(m_pSystemAPI->iEngineAPI->iGetProjectConfig().sBootConfig.strApplicationName.c_str());
    if(m_pSystemAPI->iProtocolAPI->iGetServerVersion() >= bc::SERVER_VERSION_V10)
    {
        QString strOri = qAppName();
        QStringList listTemp = strOri.split("_");
        if(listTemp.length() >= 2)
        {
            strApplicationName += ":" + listTemp[1];
        }

    }
    m_pSystemAPI->iEngineAPI->iGetProjectConfig().sBootConfig.strApplicationName = strApplicationName.toLocal8Bit().toStdString();
    qApp->setApplicationName(strApplicationName);

    int ret = loadMainFramePlugin();

    if(m_pMainFramePlugin == NULL)
    {
        BCMessageBox message(QStringLiteral("未知原因创建主窗口失败！"));
        message.addButton(QStringLiteral("确定"));
        message.exec();
        return -1;
    }


    if(ret < 0)
    {
        BCMessageBox message(QStringLiteral("加载主窗口失败！"));
        message.addButton(QStringLiteral("确定"));
        message.exec();
        return -1;
    }
    else if(ret == LoadEngineError)
    {
        BCMessageBox message(QStringLiteral("加载3D引擎失败！"));
        message.addButton(QStringLiteral("确定"));
        message.exec();
        return -1;
    }
    else if(ret == CreateConfigError)
    {
        BCMessageBox message(QStringLiteral("创建数据文件失败！"));
        message.addButton(QStringLiteral("确定"));
        message.exec();
        return -1;
    }


    if(!loadLoginPlugin())
    {
        BCMessageBox message(QStringLiteral("创建登陆窗口失败！"));
        message.addButton(QStringLiteral("确定"));
        message.exec();
        return -1;
    }

    m_pSystemAPI->iEngineAPI->iAddEventProcessor(this);

    m_pStackLayout = new QStackedLayout;
    m_pStackLayout->setStackingMode(QStackedLayout::StackAll);
    m_pStackLayout->setContentsMargins(0,0,0,0);
    m_pStackLayout->setSpacing(0);

    m_pRenderWidget = new QWidget(m_pMainFramePlugin->GetMainWindow());
    m_pRenderWidget->setLayout(m_pStackLayout);

    loadDockPlugin();

    loadMenuPlugin();

    loadToolPlugin();

    loadScenePlugin();

    qDebug()<<"load Scene Plugin";

    loadSceneControlPlugin();

    qDebug()<<"load Scene Control Plugin";

    m_pMainFramePlugin->InsertMainStackWidget(bc::SceneStackIndex, m_pRenderWidget);
    m_pMainFramePlugin->UpdateCurrentMainStackWidget();
    m_pMainFramePlugin->GetMainFrame()->hide();

    std::stable_sort(m_PluginList.begin(), m_PluginList.end(), PluginCompFunc);

    qDebug()<<"load Plugin Done";
    m_bPluginLoad = true;

    ICommand tCommand(COMMAND_PLUGIN_LOAD);
    iSendCommand(NULL, &tCommand);

    return 0;
}

void BCPluginManager::iMousePress(int x, int y)
{
    m_bMousePress = true;
    m_PressPoint = QPoint(x, y);
}

void BCPluginManager::iMouseMove()
{
    if(m_bMousePress)
    {
        if((QCursor::pos()-m_PressPoint).y() > 0)
        {
            m_pMainFramePlugin->GetMainFrame()->move(QCursor::pos()-m_PressPoint);
        }
        else
        {
            m_pMainFramePlugin->GetMainFrame()->move((QCursor::pos()-m_PressPoint).x(), 0);
        }
    }
}

void BCPluginManager::iMouseRelease(int x, int y)
{
    m_bMousePress = false;
}

bc::EventReturnType_e BCPluginManager::iProcessEvent(const bc::BCEvent& tEvent)
{
    switch(tEvent.eMessage)
    {
    case bc::EventType_e::EVENT_SCENE_OPEN:
    {
        ICommand tCommand(COMMAND_SCENE_FILE_LOAD);
        iSendCommand(NULL, &tCommand);
        qDebug()<<"Scene File Load";
    }
        break;
    case bc::EventType_e::EVENT_SCENE_LOAD:
    {
        ICommand tCommand(COMMAND_SCENE_LOAD);
        iSendCommand(NULL, &tCommand);
        qDebug()<<"Scene Load";

        tCommand.eMessage = COMMAND_LOAD_DATA;
        iSendCommand(NULL, &tCommand);
        qDebug()<<"Load Data";
    }
        break;
    case bc::EventType_e::EVENT_SCENE_VIEW_SWITCHED:
    {
        ICommand tCommand(COMMAND_SCENE_SWITCHED);
        iSendCommand(NULL, &tCommand);
        qDebug()<<"Scene Switch";
    }
        break;
    case bc::EventType_e::EVENT_RUN_WEB_JS:
    {
        iEmitSignalRunJavaScript(QString::fromStdString(tEvent.strWebJsParam.c_str()));
    }
        break;
    }

    foreach(IWidgetPlugin* pPlugin, m_PluginList)
    {
        pPlugin->iProcessEvent(tEvent);
    }

    return bc::EventReturnType_e::NONE;
}

int BCPluginManager::iGetProcessEventSort()
{
    return (int)(bc::ProcessEventSort_e::CoreAPISort) + 1;
}

void BCPluginManager::iSetWindowSize(QSize size)
{
    m_WindowSize = size;
    m_bFullScreen = false;
}

void BCPluginManager::iSetWindowSize(float fWidth, float fHeight)
{
    m_WindowSize = QSize(fWidth, fHeight);
    m_bFullScreen = false;
}

float BCPluginManager::iGetWindowWidth()
{
    return (float)m_WindowSize.width();
}

float BCPluginManager::iGetWindowHeight()
{
    return (float)m_WindowSize.height();
}

void BCPluginManager::iSendCommand(IWidgetPlugin *pSender, ICommand *pCommand)
{
    RETURN_IF_PTR_IS_NULL(pCommand);

    if(pCommand->eMessage == COMMAND_RELOAD_WEB)
    {
        emit sigReloadWeb();
    }

    foreach(IWidgetPlugin *pPlugin, m_PluginList)
    {
        if(pSender != pPlugin)
        {
            pPlugin->iExecuteCommand(pCommand);
        }
    }
}

void BCPluginManager::initSize()
{
    m_bFullScreen = m_pSystemAPI->iEngineAPI->iGetProjectConfig().sBootConfig.bFullScreen;
    m_WindowSize = QSize(m_pSystemAPI->iEngineAPI->iGetProjectConfig().sBootConfig.nOutputWidth,
                         m_pSystemAPI->iEngineAPI->iGetProjectConfig().sBootConfig.nOutputHeight);

    if(m_bFullScreen)
    {
        m_pMainFramePlugin->GetMainFrame()->winId();
#ifndef BCQT6
        QWindowsWindowFunctions::setHasBorderInFullScreen(m_pMainFramePlugin->GetMainFrame()->windowHandle(), true);
#endif
        m_pMainFramePlugin->GetMainFrame()->showFullScreen();
#ifndef __GNUC__
        m_WindowSize = QSize(m_pMainFramePlugin->GetMainFrame()->width(),m_pMainFramePlugin->GetMainFrame()->height());
#endif

    }
    else
    {
        m_pMainFramePlugin->GetMainFrame()->resize(m_WindowSize);
        m_pMainFramePlugin->GetMainFrame()->move(0, 0);
        m_pMainFramePlugin->GetMainFrame()->show();
    }
}

void BCPluginManager::iStartGame()
{
    if(m_bPluginLoad)
    {
        initSize();

        if(m_pSystemAPI->iEngineAPI->iGetProjectConfig().sBootConfig.strWebUrl.empty() || m_pSystemAPI->iEngineAPI->iGetProjectConfig().sBootConfig.bBackgroundRender
                || !m_pSystemAPI->iEngineAPI->iGetProjectConfig().sBootConfig.bShowLogin)
        {
            m_pMainFramePlugin->StartGame();
        }

        if(!m_pSystemAPI->iEngineAPI->iGetProjectConfig().sBootConfig.bBackgroundRender && !m_pSystemAPI->iEngineAPI->iGetProjectConfig().sBootConfig.strWebUrl.empty())
        {
            //本地也启用websocket进行跟网页数据交互
            ServerManager::GetInstance()->SetSystemAPI(m_pSystemAPI);
            ServerManager::GetInstance()->StartLocalWebSocketServer();
            ICommand tCommand(COMMAND_START_LOAD_WEB);
            iSendCommand(NULL, &tCommand);
        }

        if(m_pSystemAPI->iEngineAPI->iGetProjectConfig().sBootConfig.bBackgroundRender)
        {
            if(!m_pSystemAPI->iEngineAPI->iGetProjectConfig().sBootConfig.bShowMainWindow)
            {
                m_pMainFramePlugin->GetMainFrame()->hide();
            }

            if(m_pSystemAPI->iEngineAPI->iGetProjectConfig().sBootConfig.bShowServerTray)
            {
                m_trayIcon = new QSystemTrayIcon(m_pMainFramePlugin->GetMainFrame());
                m_trayIcon->setIcon(QIcon(":/resource/image/icon_server.png"));     //设置图标
                m_trayIcon->setToolTip(QStringLiteral("3D引擎服务"));    //设置鼠标放上去显示的信息
                QMenu *menu = new QMenu(m_pMainFramePlugin->GetMainFrame());			 //右键菜单
                menu->addAction(QStringLiteral("退出"));                       //这几个action是在设计师下添加的
                m_trayIcon->setContextMenu(menu); 			 //设置右键菜单
                connect(menu,SIGNAL(triggered(QAction*)),this,SLOT(slotTrigerMenu(QAction*)));
                m_trayIcon->show();
            }
        }
    }
}

IWidgetPlugin* BCPluginManager::iGetPlugin(PluginType type)
{
    foreach(IWidgetPlugin *pPlugin, m_PluginList)
    {
        if(pPlugin->GetPluginType() == type)
        {
            return pPlugin;
        }
    }

    return NULL;
}

QList<IWidgetPlugin*> BCPluginManager::iGetPlugins(PluginType type)
{
    QList<IWidgetPlugin*> pluginList;
    foreach(IWidgetPlugin *pPlugin, m_PluginList)
    {
        if(pPlugin->GetPluginType() == type)
        {
            pluginList.append(pPlugin);
        }
    }

    return pluginList;
}

void BCPluginManager::loadSceneControlPlugin()
{
    QDir pluginsDir(qApp->applicationDirPath());
    pluginsDir.cd("plugins/scenecontrol");

    foreach (QString fileName, pluginsDir.entryList(QDir::Files))
    {
        QPluginLoader pluginLoader(pluginsDir.absoluteFilePath(fileName));
        QObject *plugin = pluginLoader.instance();
        if (plugin)
        {
#ifdef __GNUC__
            if(!m_strPrefix.isEmpty() && !fileName.startsWith("lib" + m_strPrefix))
            {
                continue;
            }
#else
            if(!m_strPrefix.isEmpty() && !fileName.startsWith(m_strPrefix))
            {
                continue;
            }
#endif

            ISceneControlPlugin *pSceneControlPlugin = qobject_cast<ISceneControlPlugin*>(plugin);
            if(pSceneControlPlugin)
            {
                m_PluginList.append(pSceneControlPlugin);
                pSceneControlPlugin->SetPluginManager(this);

                QWidget *pWidget  = pSceneControlPlugin->CreateWidget(m_pRenderWidget);
                if(pWidget)
                {
                    m_pStackLayout->addWidget(pWidget);
                    m_pStackLayout->setCurrentWidget(pWidget);
                    return;
                }
            }

        }
    }
}

bool BCPluginManager::loadLoginPlugin()
{
    QDir pluginsDir(qApp->applicationDirPath());
    pluginsDir.cd("plugins/login");

    foreach (QString fileName, pluginsDir.entryList(QDir::Files))
    {
        QPluginLoader pluginLoader(pluginsDir.absoluteFilePath(fileName));
        QObject *plugin = pluginLoader.instance();
        if (plugin)
        {
#ifdef __GNUC__
            if(!m_strPrefix.isEmpty() && !fileName.startsWith("lib" + m_strPrefix))
            {
                continue;
            }
#else
            if(!m_strPrefix.isEmpty() && !fileName.startsWith(m_strPrefix))
            {
                continue;
            }
#endif
            ILoginPlugin *pPlugin = qobject_cast<ILoginPlugin *>(plugin);
            if (pPlugin)
            {
                pPlugin->SetPluginManager(this);
                if(pPlugin->Init())
                {
                    m_pLoginPlugin  = pPlugin;
                    return true;
                }
            }
        }
    }

    return false;
}

void BCPluginManager::loadScenePlugin()
{
    QDir pluginsDir(qApp->applicationDirPath());
    pluginsDir.cd("plugins/scene");

    foreach (QString fileName, pluginsDir.entryList(QDir::Files))
    {
        QPluginLoader pluginLoader(pluginsDir.absoluteFilePath(fileName));
        QObject *plugin = pluginLoader.instance();
        if (plugin)
        {
#ifdef __GNUC__
            if(!m_strPrefix.isEmpty() && !fileName.startsWith("lib" + m_strPrefix))
            {
                continue;
            }
#else
            if(!m_strPrefix.isEmpty() && !fileName.startsWith(m_strPrefix))
            {
                continue;
            }
#endif
            IScenePlugin *pScenePlugin = qobject_cast<IScenePlugin*>(plugin);
            if(pScenePlugin)
            {
                m_PluginList.append(pScenePlugin);
                pScenePlugin->SetPluginManager(this);

                QWidget *pWidget  = pScenePlugin->CreateWidget(m_pRenderWidget);

                if(pWidget)
                {
                    m_pStackLayout->addWidget(pWidget);
                }

                return;
            }
        }
    }
}

void BCPluginManager::loadDockPlugin()
{
    QDir pluginsDir(qApp->applicationDirPath());
    pluginsDir.cd("plugins/dock");

    std::vector<IDockPlugin*> vecDockPlugin;
    foreach (QString fileName, pluginsDir.entryList(QDir::Files))
    {
        QPluginLoader pluginLoader(pluginsDir.absoluteFilePath(fileName));
        QObject *plugin = pluginLoader.instance();
        if (plugin)
        {
#ifdef __GNUC__
            if(!m_strPrefix.isEmpty() && !fileName.startsWith("lib" + m_strPrefix))
            {
                continue;
            }
#else
            if(!m_strPrefix.isEmpty() && !fileName.startsWith(m_strPrefix))
            {
                continue;
            }
#endif
            IDockPlugin *pPlugin = qobject_cast<IDockPlugin *>(plugin);
            if (pPlugin)
            {
                vecDockPlugin.push_back(pPlugin);
            }
        }
    }

    std::sort(vecDockPlugin.begin(), vecDockPlugin.end(), DockCompFunc);

    for(int i = 0; i < vecDockPlugin.size(); ++i)
    {
        IDockPlugin *pPlugin = vecDockPlugin[i];

        pPlugin->SetPluginManager(this);
        pPlugin->CreateWidget(m_pMainFramePlugin->GetMainWindow());
        QDockWidget *pDock    = pPlugin->GetDockWidget();
        pPlugin->SetDockTitleWidget(m_pMainFramePlugin->CreateDockTitleWidget(pDock, pPlugin->Title()));

        m_pMainFramePlugin->AddDockWidget(pPlugin->Area(), pPlugin);

        m_PluginList.append(pPlugin);
    }
}

int BCPluginManager::loadMenuPlugin()
{
    QDir pluginsDir(qApp->applicationDirPath());
    pluginsDir.cd("plugins/menu");

    foreach (QString fileName, pluginsDir.entryList(QDir::Files))
    {
        QPluginLoader pluginLoader(pluginsDir.absoluteFilePath(fileName));
        QObject *plugin = pluginLoader.instance();
        if (plugin)
        {
#ifdef __GNUC__
            if(!m_strPrefix.isEmpty() && !fileName.startsWith("lib" + m_strPrefix))
            {
                continue;
            }
#else
            if(!m_strPrefix.isEmpty() && !fileName.startsWith(m_strPrefix))
            {
                continue;
            }
#endif
            IMenuPlugin *pMenuPlugin = qobject_cast<IMenuPlugin*>(plugin);
            if(pMenuPlugin)
            {
                m_PluginList.append(pMenuPlugin);

                pMenuPlugin->SetPluginManager(this);
                pMenuPlugin->CreateWidget(m_pMainFramePlugin->GetMainFrame());

                QList<QPushButton*> buttonList;
                QMenu *pFileMenu    = pMenuPlugin->FileMenu();
                m_pMainFramePlugin->SetFileMenu(pFileMenu);
                QPushButton *pFileButton = new QPushButton(pFileMenu->title(), m_pMainFramePlugin->GetMainFrame());
                pFileButton->setMenu(pFileMenu);
                buttonList.append(pFileButton);

                QMenu *pEditMenu    = pMenuPlugin->EditMenu();
                m_pMainFramePlugin->SetEditMenu(pEditMenu);
                QPushButton *pEditButton = new QPushButton(pEditMenu->title(), m_pMainFramePlugin->GetMainFrame());
                pEditButton->setMenu(pEditMenu);
                buttonList.append(pEditButton);

                QMenu *pViewMenu    = pMenuPlugin->ViewMenu();
                m_pMainFramePlugin->SetViewMenu(pViewMenu);
                QPushButton *pViewButton = new QPushButton(pViewMenu->title(), m_pMainFramePlugin->GetMainFrame());
                pViewButton->setMenu(pViewMenu);
                buttonList.append(pViewButton);

                QMenu *pToolMenu    = pMenuPlugin->ToolsMenu();
                m_pMainFramePlugin->SetToolsMenu(pToolMenu);
                QPushButton *pToolButton = new QPushButton(pToolMenu->title(), m_pMainFramePlugin->GetMainFrame());
                pToolButton->setMenu(pToolMenu);
                buttonList.append(pToolButton);
                QMenu *pPluginMenu    = pMenuPlugin->PluginMenu();
                m_pMainFramePlugin->SetPluginMenu(pPluginMenu);
                QPushButton *pPluginButton = new QPushButton(pPluginMenu->title(), m_pMainFramePlugin->GetMainFrame());
                pPluginButton->setMenu(pPluginMenu);
                buttonList.append(pPluginButton);

                QMenu *pHelpMenu    = pMenuPlugin->HelpMenu();
                m_pMainFramePlugin->SetHelpMenu(pHelpMenu);
                QPushButton *pHelpButton = new QPushButton(pHelpMenu->title(), m_pMainFramePlugin->GetMainFrame());
                pHelpButton->setMenu(pHelpMenu);
                buttonList.append(pHelpButton);

                foreach(QPushButton *pButton, buttonList)
                {
                    //pButton->setFlat(true);
                    pButton->setWhatsThis("menuButton");
                    m_pMainFramePlugin->AddMenuButton(pButton);
                }
            }
        }
    }

    return 0;
}

int BCPluginManager::loadToolPlugin()
{
    QDir pluginsDir(qApp->applicationDirPath());
    pluginsDir.cd("plugins/tool");

    foreach (QString fileName, pluginsDir.entryList(QDir::Files))
    {
        QPluginLoader pluginLoader(pluginsDir.absoluteFilePath(fileName));
        QObject *plugin = pluginLoader.instance();
        if (plugin)
        {
#ifdef __GNUC__
            if(!m_strPrefix.isEmpty() && !fileName.startsWith("lib" + m_strPrefix))
            {
                continue;
            }
#else
            if(!m_strPrefix.isEmpty() && !fileName.startsWith(m_strPrefix))
            {
                continue;
            }
#endif
            IToolPlugin *pToolPlugin = qobject_cast<IToolPlugin*>(plugin);
            if(pToolPlugin)
            {
                m_PluginList.append(pToolPlugin);

                pToolPlugin->SetPluginManager(this);
                pToolPlugin->CreateWidget(m_pMainFramePlugin->GetMainWindow());
                m_pMainFramePlugin->AddToolBar(pToolPlugin->ToolBarArea(), pToolPlugin->GetToolBar());
            }
        }
    }

    return 0;
}

int BCPluginManager::loadMainFramePlugin()
{
    QDir pluginsDir(qApp->applicationDirPath());
    pluginsDir.cd("plugins/mainframe");

    foreach (QString fileName, pluginsDir.entryList(QDir::Files))
    {
        QPluginLoader pluginLoader(pluginsDir.absoluteFilePath(fileName));
        QObject *plugin = pluginLoader.instance();
        if (plugin )
        {
#ifdef __GNUC__
            if(!m_strPrefix.isEmpty() && !fileName.startsWith("lib" + m_strPrefix))
            {
                continue;
            }
#else
            if(!m_strPrefix.isEmpty() && !fileName.startsWith(m_strPrefix))
            {
                continue;
            }
#endif
            IMainFramePlugin *pPlugin = qobject_cast<IMainFramePlugin *>(plugin);
            if (pPlugin)
            {
                m_pMainFramePlugin  = pPlugin;
                m_PluginList.append(m_pMainFramePlugin);
                m_pMainFramePlugin->SetPluginManager(this);
                return m_pMainFramePlugin->CreateFrame();
            }
        }
    }

    return -1;
}

void BCPluginManager::iSetDataChange(bool bChange, QString strID)
{
    if(strID.isEmpty())
    {
        QMap<QString, bool>::iterator iter = m_mDataChange.begin();
        while (iter != m_mDataChange.end())
        {
            m_mDataChange[iter.key()] = bChange;
            iter++;
        }
    }
    else
    {
        m_mDataChange[strID] = bChange;
    }
}

bool BCPluginManager::iIsDataChange(QString strID)
{
    if(strID.isEmpty())
    {
        QMap<QString, bool>::iterator iter = m_mDataChange.begin();
        while (iter != m_mDataChange.end())
        {
            if(iter.value())
            {
                return true;
            }
            iter++;
        }
    }
    else
    {
        if(m_mDataChange.find(strID) != m_mDataChange.end())
        {
            return m_mDataChange[strID];
        }
    }

    return false;
}

void BCPluginManager::iAddWebClientBrowser(QWidget *pParent,QString strUrl)
{
    if(m_mapAllWidgets.contains(pParent))
    {
        return;
    }
    m_mapAllWidgets[pParent] = strUrl;
    QString strOriUrl = strUrl;
    if(strUrl == "")
    {
        return;
    }

    bool bRemoteRender = iGetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.bBackgroundRender;
    if(bRemoteRender)
    {
        //远程渲染不加载网页
        return;
    }

    //判断是不是网络地址
    if(!strUrl.startsWith("http://") && !strUrl.startsWith("https://"))
    {
        QString strDataPath = QString::fromLocal8Bit(iGetSystemAPI()->iEngineAPI->iGetProjectConfig().sPathConfig.strDataDir.c_str()).replace("\\", "/");
        strUrl =  "file:///" + strDataPath + "/" + strUrl;
    }
    QString strDataHost = QString::fromLocal8Bit(iGetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.strServerIp.c_str());
    if(strUrl.contains("?"))
    {
        strUrl += QString("&isClient=1");
    }
    else
    {
        strUrl += QString("?isClient=1");
    }

    strUrl += QString("&commonHost=") + strDataHost;
    strUrl += QString("&width=") + QString::number(iGetWindowWidth());
    strUrl += QString("&height=") + QString::number(iGetWindowHeight());
    strUrl += QString("&isShowLogin=") + QString::number(iGetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.bShowLogin?1:0);
    strUrl += QString("&serverVersion=") + QString::number(iGetSystemAPI()->iProtocolAPI->iGetServerVersion());
    strUrl += QString("&cmd_port=") + QString::number(iGetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.nQtCmdPort);

    QString strFinalUrl = strUrl;
    if(iGetSystemAPI()->iProjectClientAPI)
    {
        strFinalUrl = QString::fromLocal8Bit(iGetSystemAPI()->iProjectClientAPI->iPreLoadWebPage(strOriUrl.toLocal8Bit().toStdString(),strUrl.toLocal8Bit().toStdString()).c_str());
    }

    strFinalUrl += "/";

    BCWebView *pView = new BCWebView(pParent, ":/resource/image/WebLoading.gif");
    m_pBCWebView = pView;

    connect(this, SIGNAL(sigReloadWeb()), pView, SLOT(slotReload()));

    connect(pView, &BCWebView::sigMousePress, [=](int nKey, QPoint point)
    {
        if(!iCheckWebCanHandleScene(point))
        {
            return;
        }
        ICommand tCommand(COMMAND_MOUSE_PRESS, (INT_PTR)(nKey), (INT_PTR)(point.x()), (INT_PTR)(point.y()));
        iSendCommand(NULL, &tCommand);
    });

    connect(pView, &BCWebView::sigMouseDbClick, [=](int nKey, QPoint point)
    {
        if(!iCheckWebCanHandleScene(point))
        {
            return;
        }
        ICommand tCommand(COMMAND_MOUSE_DBCLICK, (INT_PTR)(nKey), (INT_PTR)(point.x()), (INT_PTR)(point.y()));
        iSendCommand(NULL, &tCommand);
    });

    connect(pView, &BCWebView::sigMouseRelease, [=](int nKey, QPoint point)
    {
        ICommand tCommand(COMMAND_MOUSE_RELEASE, (INT_PTR)(nKey), (INT_PTR)(point.x()), (INT_PTR)(point.y()));
        iSendCommand(NULL, &tCommand);
    });

    connect(pView, &BCWebView::sigWheel, [=](QPoint point, QPoint angleDelta)
    {
        if(!iCheckWebCanHandleScene(point))
        {
            return;
        }
        ICommand tCommand(COMMAND_MOUSE_WHEEL, (INT_PTR)(point.x()), (INT_PTR)(point.y()), (INT_PTR)(angleDelta.x()), (INT_PTR)(angleDelta.y()));
        iSendCommand(NULL, &tCommand);
    });

    connect(pView, &BCWebView::sigKeyPress, [=](int nKey)
    {
        if(nKey == Qt::Key_F6)
        {
            ICommand _tCommand(COMMAND_RELOAD_WEB);
            iSendCommand(NULL, &_tCommand);
        }
        if(nKey == Qt::Key_F11)
        {
            //全屏切换
            ICommand tCommand(COMMAND_APP_SHOW_STATUSBAR);
            iSendCommand(nullptr, &tCommand);
        }
        if(nKey == Qt::Key_F10)
        {
            return;
        }
        if(!m_bCanKeyDown)
        {
            return;
        }
        ICommand tCommand(COMMAND_KEY_PRESS, (INT_PTR)(nKey));
        iSendCommand(NULL, &tCommand);
    });

    connect(pView, &BCWebView::sigKeyRelease, [=](int nKey)
    {
        if(!m_bCanKeyDown)
        {
            return;
        }

        ICommand tCommand(COMMAND_KEY_RELEASE, (INT_PTR)(nKey));
        iSendCommand(NULL, &tCommand);
    });

    QString strWebChannel = strFinalUrl;
    strWebChannel = strWebChannel.section("/", -1);
    strWebChannel = strWebChannel.section(".", 0, 0);
    strWebChannel += "Channel";
    pView->SetWebChannelObject(this, strWebChannel);

    pView->load(QUrl(strFinalUrl));

    QVBoxLayout *pVLayout = new QVBoxLayout;
    pVLayout->setSpacing(0);
    pVLayout->setContentsMargins(0, 0, 0, 0);
    pVLayout->addWidget(pView);
    pParent->setLayout(pVLayout);
}

bool BCPluginManager::iCheckWebCanHandleScene(QPoint &point)
{
//    if(!m_pHCWebView)
//    {
//        return true;
//    }

//    if(!m_pHCWebView->isVisible())
//    {
//        return true;
//    }

    bool bCan = true;

    for(QMap<QString,QList<WebCoverPos>>::Iterator it = m_mapWebCoverPos.begin() ; it != m_mapWebCoverPos.end() ; ++it)
    {
        QList<WebCoverPos> &listWebCoverPos = it.value();
        for(int i = 0 ; i < listWebCoverPos.size() ; i++)
        {
            int x = listWebCoverPos[i].leftTop.x();
            int y = listWebCoverPos[i].leftTop.y();
            int width = listWebCoverPos[i].rightBottom.x() - x;
            int height = listWebCoverPos[i].rightBottom.y() - y;
            QRect rect(x,y,width,height);
            if(rect.contains(point))
            {
                bCan = false;
                break;
            }
        }
    }
    return bCan;
}

void BCPluginManager::iEmitSignalRunJavaScript(QString strJson)
{
    emit sigRunJsFromCore(strJson);
}

void BCPluginManager::iWebRunJavaScript(QString strJson)
{
//    bool bRemoteRender = iGetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.bBackgroundRender;
//    if(bRemoteRender)
//    {
//        iSendToWebSocket(strJson);
//    }
//    else
//    {
//        if(m_pHCWebView)
//        {
//            m_pHCWebView->RunJavaScript(strJson);
//        }
//    }

    iSendToWebSocket(strJson);
}

void BCPluginManager::iSendToWebSocket(QString strJson)
{
    QByteArray ba = strJson.toUtf8();
    char *pData=ba.data();
    int nLength=strJson.length();

    //    qDebug()<<"BCPluginManager::iSendToWebSocket";
    ServerManager::GetInstance()->ExecCallback(ENUM_CB_SEND_BY_WEBSOCKET, (INT_PTR)pData, (INT_PTR)nLength);
}

void BCPluginManager::slotFromWebFun(QJsonValue WebParam)
{
    QJsonDocument doc;
    doc.setObject(WebParam.toObject());

    QByteArray byte_array = doc.toJson(QJsonDocument::Compact);

    WebFunParamList_T stParamList;

    if(praseWebFunParam(stParamList, byte_array) == -1)
        return;

    matchTaskFunClass(stParamList);
}

void BCPluginManager::slotRunJSFromCore(QString strJson)
{
    iWebRunJavaScript(strJson);
}

void BCPluginManager::slotTrigerMenu(QAction* act)
{
    if(act->text() == QStringLiteral("退出"))
    {
        ICommand tCommand(COMMAND_APP_CLOSE);
        iSendCommand(nullptr, &tCommand);
    }
}

int BCPluginManager::praseWebFunParam(WebFunParamList_T &stParam, const QByteArray &strjson)
{
    int nRet = 0;
    QJsonParseError json_error;
    QJsonDocument parse_doucment = QJsonDocument::fromJson(strjson, &json_error);
    if(json_error.error == QJsonParseError::NoError)
    {
        if(parse_doucment.isObject())
        {
            QJsonObject obj = parse_doucment.object();
            if(obj.contains("strFunName"))
            {
                stParam.strFunName = obj.value("strFunName").toString();
            }
            if(obj.contains("id"))
            {
                stParam.strID = obj.value("id").toString();
            }
            if(obj.contains("Param"))
            {
                QJsonObject subObj = obj.value("Param").toObject();
                if(subObj.contains("ParamList"))
                {
                    QJsonValue arrayValue = subObj.value("ParamList");
                    stParam.jsonData = arrayValue;
                    if(arrayValue.isArray())
                    {
                        QJsonArray subArray = arrayValue.toArray();
                        for(int i = 0; i < subArray.size(); i++)
                        {
                            QJsonObject paramobj = subArray.at(i).toObject();
                            QJsonValue paramvalue = paramobj["param"];

                            QVariant param;
                            if(paramvalue.isString())
                            {
                                param = paramvalue.toString();
                                stParam.vecParamList.push_back(param);
                            }
                            else if(paramvalue.isDouble())//这里是int类型
                            {
                                float fDelta = paramvalue.toDouble() - paramvalue.toInt();
                                if(qAbs(fDelta) > 0)
                                {
                                    param = paramvalue.toDouble();
                                }
                                else
                                {
                                    param = paramvalue.toInt();
                                }
                                stParam.vecParamList.push_back(param);
                            }
                            else if(paramvalue.isBool())
                            {
                                param = paramvalue.toBool();
                                stParam.vecParamList.push_back(param);
                            }
                            else if(paramvalue.isObject())
                            {
                                param = paramvalue.toObject();
                                stParam.vecParamList.push_back(param);
                            }
                            else if(paramvalue.isArray())
                            {
                                param = paramvalue.toArray();
                                stParam.vecParamList.push_back(param);
                            }
                            else
                            {
                                //else;
                            }
                        }
                    }

                }
            }
        }
    }
    else
        nRet = -1;

    return nRet;
}

void BCPluginManager::matchTaskFunClass(WebFunParamList_T &param)
{
    //处理默认的
    bool bAccept = handleDefaultWebCommand(param);
    if(!bAccept)
    {
        //传递到VS中
        QJsonObject json;
        json.insert("key", param.strFunName);
        json.insert("id", param.strID);
        json.insert("data", param.jsonData);

        std::string strJson = QJsonDocument(json).toJson().toStdString();
        bc::BCEvent _tEvent(-100000, (bc::EVENT_PARAM)strJson.data(), 0, 0);
        if(iGetSystemAPI()->iProjectClientAPI)
        {
            iGetSystemAPI()->iProjectClientAPI->iExecuteEvent(_tEvent);
        }

    }

    //被覆盖的位置
    if(param.strFunName == "SendCoverScenePos")
    {
        QString strClient;
        QStringList list = param.strID.split(":");
        if(list.size() >= 2)
        {
            strClient = list[1];
        }
        QList<WebCoverPos> &listWebCoverPos = m_mapWebCoverPos[strClient];
        listWebCoverPos.clear();
        QJsonParseError json_error;
        QJsonDocument parse_doucment = QJsonDocument::fromJson(param.vecParamList[0].toString().toUtf8(), &json_error);
        if(json_error.error == QJsonParseError::NoError) {
            if(parse_doucment.isArray())
            {
                QJsonArray array = parse_doucment.array();
                foreach (const QJsonValue & value, array)
                {
                    QJsonObject obj = value.toObject();
                    WebCoverPos stPos;
                    stPos.leftTop = QPoint(obj["left"].toInt(),obj["top"].toInt());
                    stPos.rightBottom = QPoint(obj["right"].toInt(),obj["bottom"].toInt());
                    listWebCoverPos.append(stPos);
                }
            }
        }
    }
}

bool BCPluginManager::handleDefaultWebCommand(WebFunParamList_T &tParam)
{
    bool bAccept = false;
    if(tParam.strFunName == "slotWebMouseMove")
    {
        bAccept = true;
    }
    else if(tParam.strFunName == "slotWebMouseDownApp")
    {
        if(tParam.vecParamList[2].toInt() == 0)
        {
            iMousePress(tParam.vecParamList[0].toInt(), tParam.vecParamList[1].toInt());
        }
        bAccept = true;
    }
    else if(tParam.strFunName == "slotWebMouseDown")
    {
        if(tParam.vecParamList[2].toInt() == 0)
        {
            iGetSystemAPI()->iEngineAPI->iMouseEvent( bc::MOUSE_LBUTTONDOWN, tParam.vecParamList[0].toInt(), tParam.vecParamList[1].toInt() );
        }
        else if(tParam.vecParamList[2].toInt() == 1)
        {
            iGetSystemAPI()->iEngineAPI->iMouseEvent( bc::MOUSE_MBUTTONDOWN, tParam.vecParamList[0].toInt(), tParam.vecParamList[1].toInt() );
        }
        else if(tParam.vecParamList[2].toInt() == 2)
        {
            iGetSystemAPI()->iEngineAPI->iMouseEvent( bc::MOUSE_RBUTTONDOWN, tParam.vecParamList[0].toInt(), tParam.vecParamList[1].toInt() );
        }

        bAccept = true;
    }
    else if(tParam.strFunName == "slotWebMouseUp")
    {
        if(tParam.vecParamList[2].toInt() == 0)
        {
            iMouseRelease(tParam.vecParamList[0].toInt(), tParam.vecParamList[1].toInt());
            iGetSystemAPI()->iEngineAPI->iMouseEvent( bc::MOUSE_LBUTTONUP, tParam.vecParamList[0].toInt(), tParam.vecParamList[1].toInt() );
        }
        else if(tParam.vecParamList[2].toInt() == 1)
        {
            iGetSystemAPI()->iEngineAPI->iMouseEvent( bc::MOUSE_MBUTTONUP, tParam.vecParamList[0].toInt(), tParam.vecParamList[1].toInt() );
        }
        else if(tParam.vecParamList[2].toInt() == 2)
        {
            iGetSystemAPI()->iEngineAPI->iMouseEvent( bc::MOUSE_RBUTTONUP, tParam.vecParamList[0].toInt(), tParam.vecParamList[1].toInt() );
        }

        bAccept = true;
    }
    else if(tParam.strFunName == "slotWebMouseDBClick")
    {
        if(tParam.vecParamList[2].toInt() == 0)
        {
            //左键双击
            iGetSystemAPI()->iEngineAPI->iMouseEvent( bc::MOUSE_LBUTTONDBLCLK, tParam.vecParamList[0].toInt(), tParam.vecParamList[1].toInt() );
        }
        else if(tParam.vecParamList[2].toInt() == 2)
        {
            iGetSystemAPI()->iEngineAPI->iMouseEvent( bc::MOUSE_RBUTTONDBLCLK, tParam.vecParamList[0].toInt(), tParam.vecParamList[1].toInt() );
        }


        bAccept = true;
    }
    else if(tParam.strFunName == "slotWebMouseWheel")
    {
        int nStep = tParam.vecParamList[2].toInt();

        int fSign = 1;
        if(nStep != 0)
        {
            fSign = nStep / qAbs(nStep);
        }

        iGetSystemAPI()->iEngineAPI->iMouseEvent( bc::MOUSE_WHEEL, tParam.vecParamList[0].toInt() * fSign, tParam.vecParamList[1].toInt() );
        bAccept = true;
    }
    else if(tParam.strFunName == "slotWebKeyDown")
    {
        if(!m_bCanKeyDown)
        {
            return true;
        }
        bAccept = false;
        char vkMask = tParam.vecParamList[0].toInt();
        bc::BCEvent tEvent(bc::EVENT_KEY, vkMask, 1, 0, 0);
        iGetSystemAPI()->iEngineAPI->iExecuteEvent(tEvent);
        if(vkMask == 'n' || vkMask == 'N') // 保存视点
        {
            bc::INode* pCameraNode = iGetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()->iGetMainCamera();
            if(pCameraNode)
            {
                QClipboard *pBoard = QApplication::clipboard();
                QString strText = QString::number(pCameraNode->iGetOrigin().x)
                        + "," + QString::number(pCameraNode->iGetOrigin().y)
                        + "," + QString::number(pCameraNode->iGetOrigin().z)
                        + "," + QString::number(pCameraNode->iGetAngles().x)
                        + "," + QString::number(pCameraNode->iGetAngles().y)
                        + "," + QString::number(pCameraNode->iGetAngles().z);
                pBoard->setText(strText);

                //给云渲染发送复制信息
                QString strWebJsParam = "Web_CopyString(')";
                strWebJsParam += strText;
                strWebJsParam += "')";
                iEmitSignalRunJavaScript(strWebJsParam);
            }
        }
        if(vkMask == 'm' || vkMask == 'M') // 抓点
        {
            bc::Vector3d vPos;
            bc::INode* pCameraNode = iGetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()->iGetMainCamera();
            if(pCameraNode)
            {
                bc::ICamera* pCamera = static_cast<bc::ICamera*>(pCameraNode->iGetComponent(bc::COMPONENT_TYPE_CAMERA));
                bc::IManipulator* pManipulator = static_cast<bc::IManipulator*>(pCameraNode->iGetComponent(bc::COMPONENT_TYPE_MANIPULATOR));
                char szPos[LENGTH_1K] = { 0 };

                bc::Vector3d vDc;
                vPos = pManipulator->iGetMousePoint();
                sprintf_s(szPos, sizeof(szPos), "%lf, %lf, %lf", vPos.x, vPos.y, vPos.z);
                qDebug()<<szPos;

                if(pCamera)
                {
                    QClipboard *pBoard = QApplication::clipboard();
                    pBoard->setText(szPos);

                    //给云渲染发送复制信息
                    QString strWebJsParam = "Web_CopyString(')";
                    strWebJsParam += QString::fromUtf8(szPos);
                    strWebJsParam += "')";
                    iEmitSignalRunJavaScript(strWebJsParam);
                }
            }
        }
        if(vkMask == 'K' || vkMask == 'L')
        {
            int nCode = 0;
            if(vkMask == 'K')
            {
                //'+'
                nCode = 43;
            }
            else
            {
                //'-'
                nCode = 45;
            }
            iGetSystemAPI()->iEngineAPI->iKeyEvent(nCode, true);
        }
        if(vkMask == VK_F10) // 调试信息显隐切换
        {
            if(iGetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.bBackgroundRender)
            {
                iGetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.bShowFrameTime = !iGetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.bShowFrameTime;
            }
        }
        if(vkMask == VK_F5) // F5 刷新
        {
            ICommand tCommand(COMMAND_RELOAD_WEB);
            iSendCommand(nullptr, &tCommand);
        }
    }
    else if(tParam.strFunName == "slotWebKeyUp")
    {
        if(!m_bCanKeyDown)
        {
            return true;
        }

        char vkMask = tParam.vecParamList[0].toInt();
        bc::BCEvent tEvent(bc::EVENT_KEY, vkMask, 0, 0, 0);
        iGetSystemAPI()->iEngineAPI->iExecuteEvent(tEvent);
        bAccept = true;
    }
    else if(tParam.strFunName == "slotAppMin")
    {
        ICommand tCommand(COMMAND_APP_MIN);
        iSendCommand(nullptr, &tCommand);
        bAccept = true;
    }
    else if(tParam.strFunName == "slotAppClose")
    {
//        ICommand tCommand(COMMAND_APP_CLOSE);
//        iSendCommand(nullptr, &tCommand);
//        bAccept = true;

          emit sigKillProcess("","");
    }
    else if(tParam.strFunName == "SendCoverScenePos")
    {
        bAccept = true;
    }
    else if(tParam.strFunName == "SendSceneCanKeyDown")
    {
        m_bCanKeyDown = tParam.vecParamList[0].toBool();
        bAccept = true;
    }
    else if(tParam.strFunName == "Web_InitScene")
    {
        if(!m_pSystemAPI->iCoreAPI->iGetViewManager()->iIsSceneLoad())
        {
            ICommand tCommand(COMMAND_START_LOAD_SCENE);
            iSendCommand(NULL, &tCommand);

            m_pMainFramePlugin->StartGame();
        }
    }
    else if(tParam.strFunName == "Web_OperateScene")
    {
        bc::INode* pCameraNode = m_pSystemAPI->iCoreAPI->iGetViewManager()->iGetCurrentView()->iGetMainCamera();
        if(pCameraNode)
        {
            bc::ICamera* pCamera = static_cast<bc::ICamera*>(pCameraNode->iGetComponent(bc::COMPONENT_TYPE_CAMERA));
            float fHeight = m_pSystemAPI->iCoreAPI->iGetViewManager()->iGetCurrentView()->iGetMainCamera()->iGetOrigin().z;
            float fSceneBaseHeight = m_pSystemAPI->iEngineAPI->iGetProjectConfig().sRenderConfig.fSceneBaseHeight;
            float fRate = fabs((double)fHeight - fSceneBaseHeight) / 10.0f;

            QString strOperate = tParam.vecParamList[0].toString();
            double fValue = tParam.vecParamList[1].toDouble();
            if(fValue == 0)
            {
                if(strOperate == "rotate_up" || strOperate == "rotate_right")
                {
                    fValue = m_pSystemAPI->iEngineAPI->iGetProjectConfig().sRenderConfig.fCameraKeyRotateSpeed;
                }
                else
                {
                    fValue = m_pSystemAPI->iEngineAPI->iGetProjectConfig().sRenderConfig.fCameraKeyMoveSpeed;
                }
            }
            if(strOperate == "rotate_right")
            {
                bc::Vector3d vAngle = pCameraNode->iGetAngles();
                vAngle.y -= fValue;
                pCameraNode->iSetAngles(vAngle);
            }
            else if(strOperate == "rotate_left")
            {
                bc::Vector3d vAngle = pCameraNode->iGetAngles();
                vAngle.y += fValue;
                pCameraNode->iSetAngles(vAngle);
            }
            else if(strOperate == "move_up")
            {
                bc::Vector3d vAngle = pCameraNode->iGetAxis()[0];
                vAngle.Normalise();
                bc::Vector3d vOrigin = pCameraNode->iGetOrigin();
                vAngle.z = 0;
                vOrigin = vOrigin.MoveForward(vAngle, fValue*fRate);
                pCameraNode->iSetOrigin(vOrigin);
            }
            else if(strOperate == "move_down")
            {
                bc::Vector3d vAngle = pCameraNode->iGetAxis()[0];
                vAngle.Normalise();
                bc::Vector3d vOrigin = pCameraNode->iGetOrigin();
                vAngle.z = 0;
                vOrigin = vOrigin.MoveForward(vAngle, -fValue*fRate);
                pCameraNode->iSetOrigin(vOrigin);
            }
            else if(strOperate == "move_left")
            {
                bc::Vector3d vAngle = pCameraNode->iGetAxis()[1];
                vAngle.Normalise();
                bc::Vector3d vOrigin = pCameraNode->iGetOrigin();
                vAngle.z = 0;
                vOrigin = vOrigin.MoveForward(vAngle, fValue*fRate);
                pCameraNode->iSetOrigin(vOrigin);
            }
            else if(strOperate == "move_right")
            {
                bc::Vector3d vAngle = pCameraNode->iGetAxis()[1];
                vAngle.Normalise();
                bc::Vector3d vOrigin = pCameraNode->iGetOrigin();
                vAngle.z = 0;
                vOrigin = vOrigin.MoveForward(vAngle, -fValue*fRate);
                pCameraNode->iSetOrigin(vOrigin);
            }
            else if(strOperate == "wheel_up")
            {
                m_pSystemAPI->iEngineAPI->iMouseEvent( bc::MOUSE_WHEEL, -1*m_pSystemAPI->iEngineAPI->iGetProjectConfig().sBootConfig.nSceneWidth/2,m_pSystemAPI->iEngineAPI->iGetProjectConfig().sBootConfig.nSceneHeight/2);
            }
            else if(strOperate == "wheel_down")
            {
                m_pSystemAPI->iEngineAPI->iMouseEvent( bc::MOUSE_WHEEL, m_pSystemAPI->iEngineAPI->iGetProjectConfig().sBootConfig.nSceneWidth/2,m_pSystemAPI->iEngineAPI->iGetProjectConfig().sBootConfig.nSceneHeight/2);
            }
            else if(strOperate == "rotate_up")
            {
                bc::Vector3d vAngle = pCameraNode->iGetAngles();
                vAngle.x -= fValue;
                pCameraNode->iSetAngles(vAngle);
            }
            else if(strOperate == "rotate_down")
            {
                bc::Vector3d vAngle = pCameraNode->iGetAngles();
                vAngle.x += fValue;
                pCameraNode->iSetAngles(vAngle);
            }
        }
        bAccept = true;
    }
    else if(tParam.strFunName == "Web_ResizeWindow")
    {
        int nWidth = tParam.vecParamList[0].toInt();
        int nHeight = tParam.vecParamList[1].toInt();
        if(m_pSystemAPI->iProjectClientAPI)
        {
            QString str = QString("Recieve Web_ResizeWindow:width=%1,height=%2").arg(QString::number(nWidth),QString::number(nHeight));
            m_pSystemAPI->iProjectClientAPI->iSaveLog(bc::LOG_QT,str.toStdString());
        }
        if(nWidth <= 0 || nHeight <= 0)
        {
            return true;
        }
        if(m_nLastWindowWidth == nWidth && m_nLastWindowHeight == nHeight)
        {
            return true;
        }
        if(m_pSystemAPI->iProjectClientAPI)
        {
            QString str = QString("Do ResizeWindoe:width=%1,height=%2").arg(QString::number(nWidth),QString::number(nHeight));
            m_pSystemAPI->iProjectClientAPI->iSaveLog(bc::LOG_QT,str.toLocal8Bit().toStdString());
        }
        m_nLastWindowWidth = nWidth;
        m_nLastWindowHeight = nHeight;

        m_WindowSize = QSize(nWidth,nHeight);
        m_pSystemAPI->iEngineAPI->iGetProjectConfig().sBootConfig.nOutputWidth = nWidth;
        m_pSystemAPI->iEngineAPI->iGetProjectConfig().sBootConfig.nOutputHeight = nHeight;
//        m_pSystemAPI->iEngineAPI->iGetProjectConfig().sBootConfig.nSceneWidth = nWidth;
//        m_pSystemAPI->iEngineAPI->iGetProjectConfig().sBootConfig.nSceneHeight = nHeight;

        ICommand tCommand(COMMAND_RESIZE_GLWIDGET, (INT_PTR)(nWidth),(INT_PTR)(nHeight));
        iSendCommand(NULL, &tCommand);

        bAccept = true;
    }

    return bAccept;
}

void BCPluginManager::getFromWebSocket(INT_PTR wParam,INT_PTR lParam)
{

    const char* pData = (const char*)wParam;
    QJsonParseError error;
    QJsonDocument message = QJsonDocument::fromJson(pData, &error);
    if (error.error) {
        qDebug() << "Failed to parse text message as JSON object:" << pData
                 << "Error is:" << error.errorString();
        return;
    } else if (!message.isObject()) {
        qDebug() << "Received JSON message that is not an object: " << pData;
        return;
    }

    //    QJsonValue json((const char*)wParam);

    emit gThis->sigFromWebFun(message.object());

}

void BCPluginManager::static_forvs_connect_fn(const std::string& strCommond,std::map<std::string,INT_PTR> mapExtraParam, void* pInData, void* pOutData)
{
    BCPluginManager *pThis = (BCPluginManager*)mapExtraParam["BCPluginManager"];
    if(strCommond == "iCheckWebCanHandleScene")
    {
        bc::Vector2 *vPos = (bc::Vector2*)pInData;
        QPoint vPoint = QPoint(vPos->x,vPos->y);
        bool bCan = pThis->iCheckWebCanHandleScene(vPoint);
        int *nOut = (int*)pOutData;
        int nResult = bCan?1:0;
        *nOut = nResult;
    }
    else if(strCommond == "iGetCanKeyEvent")
    {
        bool bCan = pThis->iGetCanKeyEvent();
        int *nOut = (int*)pOutData;
        int nResult = bCan?1:0;
        *nOut = nResult;
    }
    else if(strCommond == "heartbeat_reject")
    {
        emit pThis->sigKillProcess(QStringLiteral("认证失败，请联系相关管理员！"),QStringLiteral("认证失败"));
    }
    else if(strCommond == "do_subscribe_client")
    {
        std::vector<std::string> *vecInParam = (std::vector<std::string>*)pInData;
        std::string strSubHostData = vecInParam->at(0);
        std::string strUserToken = vecInParam->at(1);
        std::string strSubId = vecInParam->at(2);

        ServerManager::GetInstance()->CreateSubScribeWebsocketClient(QString::fromLocal8Bit(strSubHostData.c_str()),QString::fromLocal8Bit(strUserToken.c_str()),QString::fromLocal8Bit(strSubId.c_str()));
    }
    else if(strCommond == "close_subscribe_connect")
    {
        ServerManager::GetInstance()->CloseSubScribeWebsocketClient();
    }
    else if(strCommond == "start_game")
    {
        if(!pThis->m_pSystemAPI->iCoreAPI->iGetViewManager()->iIsSceneLoad())
        {
            ICommand tCommand(COMMAND_START_LOAD_SCENE);
            pThis->iSendCommand(NULL, &tCommand);

            pThis->m_pMainFramePlugin->StartGame();
        }
    }
    else if(strCommond == "send_web_commond")
    {
        std::string *strCommond = (std::string*)pInData;
        std::string *strWebID = (std::string*)pOutData;
        if(pThis->m_pSystemAPI->iEngineAPI->iGetProjectConfig().sBootConfig.bBackgroundRender)
        {
            pThis->iEmitSignalRunJavaScript(QString::fromStdString(*strCommond));
        }
        else
        {
            ServerManager::GetInstance()->SendToWebJsData(QString::fromStdString(*strCommond),QString::fromStdString(*strWebID));
        }
    }
}

void BCPluginManager::slotKillProcess(QString strInfo,QString strTitle)
{
    if(!strInfo.isEmpty())
    {
        //kill进程
        BCMessageBox message(strInfo,strTitle);
        message.addButton(QStringLiteral("确定"));
        message.exec();
    }
#ifdef _WIN32	
    //杀掉进程
    QProcess p;
    int pid = _getpid();
    QString cmd = QString("taskkill /F /PID %1 /T").arg(pid);
    WinExec(cmd.toLocal8Bit(), SW_HIDE);
#endif
	
//    system(cmd.toLocal8Bit());
}
