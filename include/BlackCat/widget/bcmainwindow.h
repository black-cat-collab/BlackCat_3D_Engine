#ifndef HCMAINWINDOW_H
#define HCMAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QDoubleSpinBox>
#include <QUndoStack>
#include <QUndoCommand>
#include <QAction>
#include <QEvent>
#include "../iwidgetplugin.h"
#include "../idockplugin.h"
#include "../blackcat_global.h"
#include "../corecommon.h"

class BCMainFrame;
class BLACKCATSHARED_EXPORT BCMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit BCMainWindow(IPluginManager *pPluginManager, QWidget *parent = 0);
    virtual ~BCMainWindow();

    QStackedWidget *mainStackedWidget() { return m_pMainStackWidget; }
    virtual void InitWidget();
    virtual void AddDockWidget(Qt::DockWidgetArea area, IDockPlugin* pDockPlugin);
    virtual void SetFrame(BCMainFrame *pFrame);
    virtual void InsertMainStackWidget(int index, QWidget *pWidget);
    virtual void SetCurrentMainStackWidget(int index);
    virtual void UpdateCurrentMainStackWidget();
    virtual bool StartGame(QString strScene = "");
    virtual bool IsSceneLoad() { return m_bSceneLoad; }
    virtual void SceneLoad();
    virtual void Close();
    virtual void SetFileMenu(QMenu *) {}
    virtual void SetEditMenu(QMenu *) {}
    virtual void SetHelpMenu(QMenu *) {}
    virtual void SetViewMenu(QMenu *) {}
    virtual void SetPluginMenu(QMenu *) {}
    virtual void SetToolsMenu(QMenu *) {}
protected:
    virtual void closeEvent(QCloseEvent *pEvent);


protected:
    IPluginManager* m_pPluginManager;
    BCMainFrame*    m_pFrame;
    QStackedWidget* m_pMainStackWidget;
    QList<IWidgetPlugin*> m_listWidgetPlugins;
    bool            m_bSceneLoad;
    int             m_MainCurrentIndex;

signals:
    void sigClose();

public slots:
    void slotSaveDone() {}
};

#endif // MAINWINDOW_H
