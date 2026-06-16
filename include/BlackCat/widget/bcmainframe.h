#ifndef HCMAINFRAME_H
#define HCMAINFRAME_H

#include "../corecommon.h"
#include <QtGui>
#include <QLayout>
#include <QStyle>
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QTextEdit>
#include <QDockWidget>
#include <QMainWindow>
#include <QMenu>
#include <QDialog>
#include <QMenuBar>
#include <QPainter>
#include <QDebug>
#include "../blackcat_global.h"

#include "../ipluginmanager.h"

class BCMainWindow;

/***************** TitleBar *****************/
class BLACKCATSHARED_EXPORT BCTitleBar : public QFrame
{
    Q_OBJECT
public:
    BCTitleBar(QWidget *parent);

    virtual void addMenuButton(QPushButton *pButton);

    virtual QList<QPushButton*> menuButton()
    {
        return m_MenuButtonList;
    }

    virtual bool resizeEnable()
    {
        return m_bResizeEnable;
    }

    virtual void setResizeEnable(bool bEnable);

    QLabel* iconLabel() { return m_pIconLabel; }
    QLabel* titleLabel() { return m_pTitleLabel; }
    QPushButton* miniumButton() { return m_pMinButton;}
    QPushButton* maximumButton() { return m_pMaxButton; }
    QPushButton* closeButton() { return m_pCloseButton; }

protected:
    virtual void mouseDoubleClickEvent(QMouseEvent *);
    virtual void mousePressEvent(QMouseEvent *);
    virtual void mouseMoveEvent(QMouseEvent *);
    virtual void mouseReleaseEvent(QMouseEvent *);

protected:
    QFrame *m_pTitleFrame;
    QHBoxLayout *m_pMenuLayout;
    QLabel *m_pTitleLabel;
    QLabel *m_pIconLabel;
    QLabel *m_pAccountIconLabel;
    QList<QPushButton*> m_MenuButtonList;
    QPushButton *m_pAccountButton;
    QPushButton *m_pMinButton;
    QPushButton *m_pMaxButton;
    QPushButton *m_pCloseButton;
    QPixmap restorePix, maxPix;
    bool m_bMaxNormal;
    bool m_bPress;
    QPoint m_ClickPos;
    QPoint m_PressPos;
    bool m_bResizeEnable;
    bool m_bMousePress;
public slots:
    void slotClickAccount();
    void showSmall();
    void showMaxRestore();

signals:
    void sigClickAccount();
};

/***************** StatusBar *****************/
class BLACKCATSHARED_EXPORT BCStatusBar : public QWidget
{
    Q_OBJECT
public:
    BCStatusBar(QWidget *parent);

    virtual void setText(QString text);

    virtual void addWidget(QWidget *pWidget);

    virtual void insertWidget(int index, QWidget *pWidget);

protected:
    QLabel *m_pStatuLabel;
    QHBoxLayout *m_pHLayout;
};

/***************** MainFrame *****************/
class BLACKCATSHARED_EXPORT BCMainFrame : public QFrame
{
public:
    BCMainFrame(IPluginManager *pPluginManager);

    virtual ~BCMainFrame() { }

    virtual void CreateWidget();

    virtual void setFlag(Qt::WindowFlags flag);

    BCMainWindow* content() const
    {
        return m_pContent;
    }

    BCTitleBar * titleBar() const
    {
        return m_pTitleBar;
    }

    BCStatusBar *statusBar() const
    {
        return m_pStatusBar;
    }

    virtual void setResizeEnable(bool bEnable)
    {
        if(m_pTitleBar)
            m_pTitleBar->setResizeEnable(bEnable);
    }

    virtual void setTitle(QString title)
    {
        if(m_pTitleBar)
            m_pTitleBar->titleLabel()->setText(title);
    }

    virtual void setAccount(QString account)
    {
        //m_pTitleBar->setAccount(account);
    }

    virtual void setRestoreGeometry(QRect geo)
    {
        m_RestoreGeometry   = geo;
    }

    virtual const QSize restoreSize() const
    {
        return m_RestoreGeometry.size();
    }

    virtual const QRect& restoreGeometry() const
    {
        return m_RestoreGeometry;
    }

    virtual void setTitleBarHidden(bool bHidden)
    {
        if(m_pTitleBar)
            m_pTitleBar->setHidden(bHidden);
    }

    virtual void setStatusBarHidden(bool bHidden)
    {
        if(m_pStatusBar)
            m_pStatusBar->setHidden(bHidden);
    }

    virtual void setStatusText(QString text)
    {
        if(m_pStatusBar)
            m_pStatusBar->setText(text);
    }

    virtual void close();

protected:
    virtual void paintEvent(QPaintEvent *pEvent);

    virtual void changeEvent(QEvent *pEvent);

    virtual void showEvent(QShowEvent *pEvent);

    virtual bool eventFilter(QObject *obj, QEvent *event);

    virtual void mousePressEvent(QMouseEvent *e);

    virtual void mouseMoveEvent(QMouseEvent *e);

    virtual void mouseReleaseEvent(QMouseEvent *);

    virtual void timerEvent(QTimerEvent *);

    virtual void closeEvent(QCloseEvent *pEvent);

    virtual void resizeEvent(QResizeEvent *pEvent);

    virtual void moveEvent(QMoveEvent *pEvent);


protected:
    IPluginManager* m_pPluginManager;
    BCTitleBar*     m_pTitleBar;
    BCMainWindow*   m_pContent;
    BCStatusBar*    m_pStatusBar;
    QPoint          m_OldPos;
    bool            m_bMouseDown;
    bool            m_bLeft;
    bool            m_bRight;
    bool            m_bBottom;
    bool            m_bTop;
    QRect           m_RestoreGeometry;
    QColor          m_BorderColor;
    int             m_nBorderWidth;
    QSize           m_ContentMinimumSize;
    bool            m_bSetCursor;
};

#endif // HCMAINFRAME_H
