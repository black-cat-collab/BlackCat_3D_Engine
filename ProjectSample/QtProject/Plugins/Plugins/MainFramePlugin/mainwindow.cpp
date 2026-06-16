#include <widget/bcmainframe.h>
#include <widget/bcmainwindow.h>
#include "mainwindow.h"

MainWindow::MainWindow(IPluginManager *pPluginManager, QWidget *parent) :
    BCMainWindow(pPluginManager, parent)
{

}

MainWindow::~MainWindow()
{

}

/*************** public ***************/
void MainWindow::InitWidget()
{
    // 中心页面 使用栈页
    m_pMainStackWidget  = new QStackedWidget(this);
    setCentralWidget(m_pMainStackWidget);

    QWidget *pWidget    = new QWidget(this);
    pWidget->setStyleSheet("background-color:#eeeeee;");
    InsertMainStackWidget(bc::EmptyStackIndex, pWidget);
}
