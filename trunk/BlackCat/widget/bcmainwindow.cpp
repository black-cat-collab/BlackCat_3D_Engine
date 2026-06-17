#include "BlackCat/widget/bcmainframe.h"
#include "BlackCat/widget/bcmainwindow.h"
#include "BlackCat/bcmessagebox.h"
#include "BlackCat/iscenecontrolplugin.h"
#include <QPluginLoader>
#include <QFileDialog>
#include <QMetaType>
#include <QToolBar>
#include <QDir>
#include <QDebug>

BCMainWindow::BCMainWindow(IPluginManager *pPluginManager, QWidget *parent) :
    QMainWindow(parent),
    m_bSceneLoad(false),
    m_pFrame(nullptr),
    m_pMainStackWidget(nullptr),
    m_pPluginManager(pPluginManager)
{
    setMouseTracking(true);

}

BCMainWindow::~BCMainWindow()
{

}

/*************** public ***************/
void BCMainWindow::InitWidget()
{
    // 中心页面 使用栈页
    m_pMainStackWidget  = new QStackedWidget(this);
    setCentralWidget(m_pMainStackWidget);

    QWidget *pWidget    = new QWidget(this);
    pWidget->setStyleSheet("background-color:#eeeeee;");
    InsertMainStackWidget(bc::EmptyStackIndex, pWidget);
}

void BCMainWindow::AddDockWidget(Qt::DockWidgetArea area, IDockPlugin* pDockPlugin)
{
    if(pDockPlugin)
    {
        addDockWidget(area, pDockPlugin->GetDockWidget());
    }
}

void BCMainWindow::SetFrame(BCMainFrame *pFrame)
{
    if(pFrame)
    {
        m_pFrame = pFrame;
    }
}

void BCMainWindow::InsertMainStackWidget(int index, QWidget *pWidget)
{
    m_pMainStackWidget->insertWidget(index, pWidget);
}

void BCMainWindow::SetCurrentMainStackWidget(int index)
{
    int nCount = m_pMainStackWidget->count();
    if(index < nCount)
    {
        m_pMainStackWidget->setCurrentIndex(index);
        m_MainCurrentIndex  = index;
    }
}

void BCMainWindow::UpdateCurrentMainStackWidget()
{
    SetCurrentMainStackWidget(bc::SceneStackIndex);
}

bool BCMainWindow::StartGame(QString strScene)
{
    QString strErrorMsg = "";
    bc::ISystemAPI* pSystemAPI = m_pPluginManager->iGetSystemAPI();

    do
    {
        QString strProjectName = pSystemAPI->iEngineAPI->iGetProjectConfig().sPathConfig.strProjectDirName.c_str();
        if(strProjectName.isEmpty())
        {
            strErrorMsg = QStringLiteral("3D项目名称project_name不能为空，请检查场景目录与文件配置！");
            break;
        }

        QString strConfigFile = (pSystemAPI->iEngineAPI->iGetProjectConfig().sPathConfig.strProjectDir + "/" +
                pSystemAPI->iEngineAPI->iGetProjectConfig().strConfigFile).c_str();
        if(!QFile::exists(strConfigFile))
        {
            qDebug() << "Not Exist File: " << strConfigFile;
            strErrorMsg = strConfigFile + "\n" + QStringLiteral("3D场景配置文件不存在，请检查场景目录与文件配置！");
            break;
        }

        qDebug() << "strConfigFile = " << strConfigFile;

        if(strScene.isEmpty())
        {
            strScene = QString::fromLocal8Bit(pSystemAPI->iEngineAPI->iGetProjectConfig().sBootConfig.strSceneFile.c_str());
        }

        qDebug() << "strScene = " << strScene;

        if(!strScene.isEmpty())
        {
             QString abScenePath = QString::fromLocal8Bit(pSystemAPI->iEngineAPI->iGetProjectConfig().sPathConfig.strScenesDir.c_str()) + "/" + strScene;
             if(!QFile::exists(abScenePath) /*&& strScene != "default.tw4"*/)
             {
                 qDebug() << "Not Exist File: " << abScenePath;
                 strErrorMsg = abScenePath + "\n" + QStringLiteral("3D场景文件不存在，请检查场景目录与文件配置！");
                 break;
             }
             qDebug() << "abScenePath = " << abScenePath;
        }
        else
        {
            strErrorMsg = QStringLiteral("配置文件scene_file不能为空，请检查场景目录与文件配置！");
            break;
        }

        SetCurrentMainStackWidget(bc::SceneStackIndex);

        ICommand tCommand(COMMAND_OPEN_SCENE);
        m_pPluginManager->iSendCommand(NULL, &tCommand);

        m_pFrame->setEnabled(false);

        return true;
    }while(0);


    BCMessageBox message(strErrorMsg);
    message.addButton(QStringLiteral("确定"));
    message.exec();

    return false;
}

void BCMainWindow::SceneLoad()
{
    m_bSceneLoad = true;
    m_pFrame->setEnabled(true);
}

void BCMainWindow::Close()
{
    //杀掉进程
#ifdef _WIN32
    int pid = qApp->applicationPid();
    QString cmd = QString("taskkill /F /PID %1").arg(pid);
    WinExec(cmd.toLocal8Bit().data(), SW_HIDE);//SW_HIDE：控制运行cmd时，不弹出cmd运行窗口
#endif


}

/*************** protected ***************/
void BCMainWindow::closeEvent(QCloseEvent *pEvent)
{
    //杀掉进程
#ifdef _WIN32
    int pid = qApp->applicationPid();
    QString cmd = QString("taskkill /F /PID %1").arg(pid);
    WinExec(cmd.toLocal8Bit().data(), SW_HIDE);//SW_HIDE：控制运行cmd时，不弹出cmd运行窗口
#endif

}

/*************** private ***************/
