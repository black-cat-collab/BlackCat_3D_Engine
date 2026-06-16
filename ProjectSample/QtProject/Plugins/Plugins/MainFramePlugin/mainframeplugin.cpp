#include "mainframeplugin.h"
#include <bcmessagebox.h>
#include <ipluginmanager.h>
#include <widget/bcmainwindow.h>
#include <widget/bcmainframe.h>
#include "mainframe.h"
#include "mainwindow.h"

MainFramePlugin::MainFramePlugin() :
    IMainFramePlugin("MainFramePlugin")
{
    m_pMainFrame = NULL;
    m_pMainWindow = NULL;
}

MainFramePlugin::~MainFramePlugin()
{

}

CreateMainFrameStatus MainFramePlugin::CreateFrame()
{
    m_pMainFrame  = new MainFrame(GetPluginManager());
    m_pMainFrame->CreateWidget();
    m_pMainWindow = (MainWindow*)(m_pMainFrame->content());
    m_pMainWindow->SetFrame(m_pMainFrame);

    return CreateSuccess;
}


void MainFramePlugin::iShutDown()
{
    qDebug()<<"iShut Down";

    if(m_pMainFrame)
    {
        delete m_pMainFrame;
    }
    else if(m_pMainWindow)
    {
        delete m_pMainWindow;
    }

//    if(m_pMainFrame)
//        m_pMainFrame->deleteLater();
//    else if(m_pMainWindow)
//        m_pMainWindow->deleteLater();
}
