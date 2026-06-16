#ifndef IMAINFRAMEPLUGIN_H
#define IMAINFRAMEPLUGIN_H
#include <QString>
#include <QMainWindow>
#include "corecommon.h"
#include "basicdefine.h"
#include <QPushButton>
#include "iwidgetplugin.h"
#include "idockplugin.h"
#include "widget/bcmainframe.h"
#include "widget/bcmainwindow.h"
#include <QCoreApplication>

class IPluginManager;
class IMainFramePlugin : public IWidgetPlugin
{
public:
    IMainFramePlugin(QString objID):IWidgetPlugin(MainFramePluginType, objID),m_bShowTitleBar(false) { }
    virtual QWidget* CreateDockTitleWidget(QDockWidget *, QString) { return nullptr; }
    virtual void SetFileMenu(QMenu *) {}
    virtual void SetEditMenu(QMenu *) {}
    virtual void SetHelpMenu(QMenu *) {}
    virtual void SetViewMenu(QMenu *) {}
    virtual void SetPluginMenu(QMenu *) {}
    virtual void SetToolsMenu(QMenu *) {}
    virtual void AddMenuButton(QPushButton *) {}
    virtual void AddToolBar(Qt::ToolBarArea, QToolBar *) {}
    virtual CreateMainFrameStatus CreateFrame() = 0;
    virtual QFrame *GetMainFrame()
    {
        return m_pMainFrame;
    }
    virtual QMainWindow *GetMainWindow()
    {
        return m_pMainWindow;
    }
    virtual void AddDockWidget(Qt::DockWidgetArea area, IDockPlugin* pDockPlugin)
    {
        RETURN_IF_PTR_IS_NULL(m_pMainWindow);
        m_pMainWindow->AddDockWidget(area, pDockPlugin);
    }
    virtual void InsertMainStackWidget(int index, QWidget *pWidget)
    {
        RETURN_IF_PTR_IS_NULL(m_pMainWindow);
        m_pMainWindow->InsertMainStackWidget(index, pWidget);
    }
    virtual void SetCurrentMainStackWidget(int index)
    {
        RETURN_IF_PTR_IS_NULL(m_pMainWindow);
        m_pMainWindow->SetCurrentMainStackWidget(index);
    }
    virtual void UpdateCurrentMainStackWidget()
    {
        RETURN_IF_PTR_IS_NULL(m_pMainWindow);
        m_pMainWindow->UpdateCurrentMainStackWidget();
    }

    virtual void StartGame(QString strScene = "")
    {
        RETURN_IF_PTR_IS_NULL(m_pMainWindow);
        m_pMainWindow->StartGame(strScene);
    }
    virtual void SceneLoad()
    {
        RETURN_IF_PTR_IS_NULL(m_pMainWindow);
        m_pMainWindow->SceneLoad();
    }
    virtual void iExecuteCommand(ICommand *pCommand)
    {
        switch(pCommand->eMessage)
        {
        case COMMAND_SIGNAL_APP_RESTORE:
        {
            RETURN_IF_PTR_IS_NULL(m_pMainFrame);
            m_pMainFrame->activateWindow();
            m_pMainFrame->showMaximized();
        }
            break;
        case COMMAND_SIGNAL_APP_CLOSE:
        {
            RETURN_IF_PTR_IS_NULL(m_pMainFrame);
            m_pMainFrame->close();
        }
            break;
        case COMMAND_APP_CLOSE:
        {
            RETURN_IF_PTR_IS_NULL(m_pMainWindow);
            m_pMainWindow->close();
        }
            break;
        case COMMAND_APP_SHOW:
        {
            RETURN_IF_PTR_IS_NULL(m_pMainFrame);
            m_pMainFrame->showNormal();
        }
            break;
        case COMMAND_APP_MIN:
        {
            RETURN_IF_PTR_IS_NULL(m_pMainFrame);
            m_pMainFrame->showMinimized();
        }
            break;
        case COMMAND_APP_MAX:
        {
            RETURN_IF_PTR_IS_NULL(m_pMainFrame);
            m_pMainFrame->showMaximized();
        }
            break;
        case COMMAND_APP_FULL_SCREEN:
        {
            RETURN_IF_PTR_IS_NULL(m_pMainFrame);
            m_pMainFrame->showFullScreen();
        }
            break;
        case COMMAND_APP_SHOW_STATUSBAR:
        {
            if(!GetPluginManager()->iGetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.bBackgroundRender)
            {
                RETURN_IF_PTR_IS_NULL(m_pMainFrame);
                m_bShowTitleBar = !m_bShowTitleBar;
                if(m_bShowTitleBar)
                {
                    m_pMainFrame->setWindowFlags(m_pMainFrame->windowFlags() & (~Qt::FramelessWindowHint));
                }
                else
                {
                    m_pMainFrame->setWindowFlags(m_pMainFrame->windowFlags() | Qt::FramelessWindowHint);

                }
                m_pMainFrame->show();
            }
        }
            break;
        case COMMAND_SCENE_LOAD:
        {
            RETURN_IF_PTR_IS_NULL(m_pMainWindow);
            m_pMainWindow->SceneLoad();
        }
            break;
        default:
            break;
        }
    }
    virtual void iShutDown() {}

protected:
    BCMainFrame *m_pMainFrame;
    BCMainWindow *m_pMainWindow;

    bool m_bShowTitleBar;


};

QT_BEGIN_NAMESPACE

#define IMainFramePlugin_iid "com.hc-project.Builder.IMainFramePlugin"

Q_DECLARE_INTERFACE(IMainFramePlugin, IMainFramePlugin_iid)
QT_END_NAMESPACE


#endif // IMAINFRAMEPLUGIN_H
