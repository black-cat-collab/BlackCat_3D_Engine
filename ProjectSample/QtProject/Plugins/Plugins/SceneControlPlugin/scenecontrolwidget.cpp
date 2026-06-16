#include "scenecontrolwidget.h"
#include "ui_scenecontrolwidget.h"
#include "scenecontrolplugin.h"
#include "Project/projectmanager.h"
#include <QDebug>

SceneControlWidget *SceneControlWidget::m_pThis = NULL;

SceneControlWidget::SceneControlWidget(float fHeight, QWidget *parent) :
    BCResolutionWidget<QWidget>(fHeight, parent), ui(new Ui::SceneControlWidget)
{
    ui->setupUi(this);

    resize(RESOLUTION_STANDARD_WIDTH, RESOLUTION_STANDARD_HEIGHT, true);

    m_pThis = this;

    init();

    AdaptChildToResolution();
}

SceneControlWidget::~SceneControlWidget()
{
    qDebug()<<"~SceneControlWidget";
    delete ui;
}

void SceneControlWidget::init()
{
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);

    m_pMainWebFrame = new QFrame(this);

    m_pMainStackLayout = new QStackedLayout;
    m_pMainStackLayout->setStackingMode(QStackedLayout::StackAll);
    m_pMainStackLayout->addWidget(m_pMainWebFrame);

    QHBoxLayout *pHLayout    = new QHBoxLayout;
    pHLayout->setSpacing(0);
    pHLayout->setContentsMargins(0,0,0,0);
    pHLayout->addLayout(m_pMainStackLayout);
    setLayout(pHLayout);

    m_pProjectManager = new ProjectManager(this);
}

void SceneControlWidget::SceneLoad()
{
    if(m_pProjectManager)
    {
        if(!g_pIPluginManager->iGetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.bShowLogin)
        {
            StartLoadWebPage();
        }
    }
}

void SceneControlWidget::StartLoadWebPage()
{
    QString strUrl = QString::fromStdString(g_pIPluginManager->iGetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.strWebUrl);
    g_pIPluginManager->iAddWebClientBrowser(m_pMainWebFrame, strUrl.toStdString().c_str());
}


void SceneControlWidget::mouseDoubleClickEvent(QMouseEvent *pEvent)
{
    RETURN_IF_PTR_IS_NULL(g_pIPluginManager);
    ICommand tCommand(COMMAND_MOUSE_DBCLICK);
    tCommand.SetParams(COMMAND_MOUSE_DBCLICK, (INT_PTR)(pEvent->button()), (INT_PTR)(pEvent->pos().x()), (INT_PTR)(pEvent->pos().y()));
    g_pIPluginManager->iSendCommand(NULL, &tCommand);
}

void SceneControlWidget::mousePressEvent(QMouseEvent *pEvent)
{
    RETURN_IF_PTR_IS_NULL(g_pIPluginManager);
    ICommand tCommand(COMMAND_MOUSE_PRESS);
    tCommand.SetParams(COMMAND_MOUSE_PRESS, (INT_PTR)(pEvent->button()), (INT_PTR)(pEvent->pos().x()), (INT_PTR)(pEvent->pos().y()));
    g_pIPluginManager->iSendCommand(NULL, &tCommand);
}

void SceneControlWidget::mouseReleaseEvent(QMouseEvent *pEvent)
{
    RETURN_IF_PTR_IS_NULL(g_pIPluginManager);
    ICommand tCommand(COMMAND_MOUSE_RELEASE);
    tCommand.SetParams(COMMAND_MOUSE_RELEASE, (INT_PTR)(pEvent->button()), (INT_PTR)(pEvent->pos().x()), (INT_PTR)(pEvent->pos().y()));
    g_pIPluginManager->iSendCommand(NULL, &tCommand);
}

void SceneControlWidget::mouseMoveEvent(QMouseEvent *pEvent)
{
    RETURN_IF_PTR_IS_NULL(g_pIPluginManager);
    ICommand tCommand(COMMAND_MOUSE_MOVE);
    tCommand.SetParams(COMMAND_MOUSE_MOVE, (INT_PTR)(pEvent->button()), (INT_PTR)(pEvent->pos().x()), (INT_PTR)(pEvent->pos().y()));
    g_pIPluginManager->iSendCommand(NULL, &tCommand);
}

void SceneControlWidget::wheelEvent(QWheelEvent *pEvent)
{
    RETURN_IF_PTR_IS_NULL(g_pIPluginManager);
    ICommand tCommand(COMMAND_MOUSE_WHEEL);
#ifdef BCQT6
    tCommand.SetParams(COMMAND_MOUSE_WHEEL, (INT_PTR)(pEvent->position().x()), (INT_PTR)(pEvent->position().y()), (INT_PTR)(pEvent->angleDelta().x()), (INT_PTR)(pEvent->angleDelta().y()));
#else
    tCommand.SetParams(COMMAND_MOUSE_WHEEL, (INT_PTR)(pEvent->pos().x()), (INT_PTR)(pEvent->pos().y()), (INT_PTR)(pEvent->angleDelta().x()), (INT_PTR)(pEvent->angleDelta().y()));
#endif
    g_pIPluginManager->iSendCommand(NULL, &tCommand);
}

void SceneControlWidget::keyPressEvent(QKeyEvent *pEvent)
{
    RETURN_IF_PTR_IS_NULL(g_pIPluginManager);
    if(!g_pIPluginManager->iGetCanKeyEvent())
    {
       return;
    }
    ICommand tCommand(COMMAND_KEY_PRESS);
    tCommand.SetParams(COMMAND_KEY_PRESS, (INT_PTR)(pEvent->key()));
    g_pIPluginManager->iSendCommand(NULL, &tCommand);
}

void SceneControlWidget::keyReleaseEvent(QKeyEvent *pEvent)
{
    RETURN_IF_PTR_IS_NULL(g_pIPluginManager);
    if(!g_pIPluginManager->iGetCanKeyEvent())
    {
       return;
    }
    ICommand tCommand(COMMAND_KEY_RELEASE);
    tCommand.SetParams(COMMAND_KEY_RELEASE, (INT_PTR)(pEvent->key()));
    g_pIPluginManager->iSendCommand(NULL, &tCommand);
}
