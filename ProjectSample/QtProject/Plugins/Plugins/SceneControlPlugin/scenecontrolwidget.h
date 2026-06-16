#ifndef SCENECONTROLWIDGET_H
#define SCENECONTROLWIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QLabel>
#include <QTimer>
#include <QMouseEvent>
#include <QStackedLayout>
#include <QUdpSocket>
#include <bcmessagebox.h>
#include <corecommon.h>
#include <bcresolutionwidget.h>

class QHttpServer;
class ProjectManager;

namespace Ui {
class SceneControlWidget;
}

class SceneControlWidget : public BCResolutionWidget<QWidget>
{
    Q_OBJECT

public:
    explicit SceneControlWidget(float fHeight, QWidget *parent = 0);
    ~SceneControlWidget();

    void SceneLoad();
    void StartLoadWebPage();

    static float ResolutionXscale(){ return m_pThis->XScale(); }
    static float ResolutionYscale() { return m_pThis->YScale(); }

protected:
    virtual void mouseDoubleClickEvent(QMouseEvent *pEvent);
    virtual void mousePressEvent(QMouseEvent *pEvent);
    virtual void mouseMoveEvent(QMouseEvent *pEvent);
    virtual void mouseReleaseEvent(QMouseEvent *pEvent);
    virtual void keyPressEvent(QKeyEvent *pEvent);
    virtual void keyReleaseEvent(QKeyEvent *pEvent);
    virtual void wheelEvent(QWheelEvent * pEvent) ;

private:
    void init();

protected:
    Ui::SceneControlWidget *ui;
    QFrame*         m_pMainWebFrame;
    QStackedLayout* m_pMainStackLayout;
    ProjectManager* m_pProjectManager;
    static SceneControlWidget *m_pThis;

};

#endif // SCENECONTROLWIDGET_H
