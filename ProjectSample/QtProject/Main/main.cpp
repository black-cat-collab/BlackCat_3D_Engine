#include <QSharedMemory>
#include <QSplashScreen>
#include <QApplication>
#include <QStandardPaths>
#include <QTranslator>
#include <QIcon>
#include <QDebug>

#include <QMessageBox>
#include <basicdefine.h>
#include <ipluginmanager.h>
#include "Include/projectheader.h"
#include "pluginmanagerclient.h"
//#include "utils.h"
#include "./qBreakpad/qbreakpadutil.h"
#include "./qtsingleapplication/qtsingleapplication.h"

#define PROPREFIX   "project"

int main(int argc, char *argv[])
{
    qputenv("QTWEBENGINE_REMOTE_DEBUGGING", "5566");
//    AddUnhandledExceptionFilter();

    int lastArgc = argc;

    int appRet = 0;

    char ARG_DISABLE_WEB_SECURITY[] = "--disable-web-security";
    int newArgc = argc+1+1;
    char** newArgv = new char*[newArgc];
    for(int i=0; i<argc; i++) {
        newArgv[i] = argv[i];
    }
    newArgv[argc] = ARG_DISABLE_WEB_SECURITY;
    newArgv[argc+1] = nullptr;
    QtSingleApplication app(newArgc, newArgv);
    if (app.isRunning())
    {
        app.sendMessage("raise_window_noop");
        return 0;
    }

    //捕捉崩溃
    QBreakpadUtil qBreakpad;

    QTranslator translator;

    translator.load(":/Resource/Translation/qt_zh_CN.qm");
    app.installTranslator(&translator);

    app.setQuitOnLastWindowClosed(true);
    app.setWindowIcon(QIcon(":/Resource/Icon/opic_64.ico"));

    PluginManagerClient *pPluginManager = PluginManagerClient::GetInstance();
    pPluginManager->iSetPluginPrefix(PROPREFIX);

    //写注册表，支持外部网页打开程序
    {
        QSettings reg("HKEY_CLASSES_ROOT\\TestDemo", QSettings::NativeFormat);
        reg.setValue(".","URL:MyApp Protocol");
        reg.setValue("URL Protocol","");
    }
    {
        QSettings reg("HKEY_CLASSES_ROOT\\TestDemo\\shell\\open\\command", QSettings::NativeFormat);
        QString strPath = QCoreApplication::applicationFilePath();
        strPath = "\"" + strPath.replace("/","\\") + "\"";
        strPath += " ";
        strPath += "\"" + QString("%1") + "\"";
        reg.setValue(".",strPath);
    }

    if(lastArgc > 1)
    {
        //带参数的，则直接不需要登录
        pPluginManager->m_bAutoLogin = true;
    }

    QSplashScreen *pSplash = NULL;
    if(1)
    {
        // 启动动画
        QPixmap pixmap(":/Resource/Image/splash.png");
        pSplash = new QSplashScreen(pixmap);
//        QDesktopWidget* desktopWidget = QApplication::desktop();
//        pSplash->move((desktopWidget->size().width() - pSplash->width()) / 2.0f,
//                      (desktopWidget->size().height() - pSplash->height()) / 2.0f);
        pSplash->show();
    }

    app.processEvents();
    if(pPluginManager->iLoadPlugin() != 0)
    {
        return -1;
    }

    //    if(pPluginManager->iGetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.bShowLogin)
    //    {
    //        // 启动登陆界面
    //        if(!pPluginManager->iLogin())
    //        {
    //            return -1;
    //        }
    //    }

    if(pSplash)
    {
        delete pSplash;
    }

//    //  设置程序锁
//    if(!pPluginManager->iGetSystemAPI()->iProtocolAPI->iGetSoftwareLockManager()->iLock(bc::SOFTWARE_LOCK_OFFLINE, "2023-06-15 00:00:00"))
//    {
//        return -1;
//    }

    if(!pPluginManager->iLogin())
    {
        return -1;
    }

    //pPluginManager->iStartGame();

    IMainFramePlugin *pMainPlugin = (IMainFramePlugin*)pPluginManager->iGetPlugin(MainFramePluginType);
    app.setActivationWindow(pMainPlugin->GetMainFrame(),true);
    appRet = app.exec();

    PluginManagerClient::Destroy();

    return appRet;
}
