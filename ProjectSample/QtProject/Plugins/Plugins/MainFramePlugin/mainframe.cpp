#include <widget/bcmainframe.h>
#include <widget/bcmainwindow.h>
#include "mainframe.h"
#include "mainwindow.h"

#define FRAME_BORDER_WIDTH 2

MainFrame::MainFrame(IPluginManager *pPluginManager) :
    BCMainFrame(pPluginManager)
{

}

MainFrame::~MainFrame()
{
    qDebug()<<"~MainFrame";
}

void MainFrame::CreateWidget()
{
    m_pTitleBar = new BCTitleBar(this);

    m_pContent = new MainWindow(m_pPluginManager);
    m_pContent->InitWidget();
    m_pContent->installEventFilter(this);

    //m_pTitleBar->setFlags(m_pContent->windowFlags());
    m_pContent->setMouseTracking(true);

    QVBoxLayout *vbox = new QVBoxLayout(this);
    vbox->addWidget(m_pTitleBar);
    vbox->setContentsMargins(FRAME_BORDER_WIDTH,FRAME_BORDER_WIDTH,FRAME_BORDER_WIDTH,FRAME_BORDER_WIDTH);
    vbox->setSpacing(0);
    vbox->addWidget(m_pContent);

    m_pStatusBar = new BCStatusBar(this);
    vbox->addWidget(m_pStatusBar);


    m_pStatusBar->setHidden(true);
    m_pTitleBar->setHidden(true);
}
