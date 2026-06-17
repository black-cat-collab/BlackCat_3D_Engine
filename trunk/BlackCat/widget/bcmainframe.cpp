#include "BlackCat/widget/bcmainframe.h"
#include "BlackCat/widget/bcmainwindow.h"
#include <QApplication>
#include <QDesktopServices>
#include <QDebug>

#define FRAME_BORDER_COLOR "#000000"
#define FRAME_BORDER_WIDTH  2

/***************** TitleBar *****************/
BCTitleBar::BCTitleBar(QWidget *parent)
    :QFrame(parent)
{
    m_bResizeEnable = false;
    m_bMousePress = false;
    setObjectName("titleWidget");

    m_pTitleFrame = new QFrame(this);

    m_pMinButton = new QPushButton(QStringLiteral("-"), m_pTitleFrame);
    m_pMaxButton = new QPushButton(QStringLiteral("口"), m_pTitleFrame);
    m_pCloseButton= new QPushButton(QStringLiteral("x"), m_pTitleFrame);

    m_pCloseButton->setFocusPolicy(Qt::NoFocus);
    m_pCloseButton->show();

    m_pMaxButton->setFocusPolicy(Qt::NoFocus);
    m_pMaxButton->hide();

    m_pMinButton->setFocusPolicy(Qt::NoFocus);
    m_pMinButton->hide();

    m_pIconLabel  = new QLabel(m_pTitleFrame);
    m_pIconLabel->raise();

    m_pTitleLabel = new QLabel(m_pTitleFrame);
    m_pTitleLabel->raise();

    QHBoxLayout *titleLayout = new QHBoxLayout;
    titleLayout->addWidget(m_pIconLabel);
    titleLayout->addWidget(m_pTitleLabel);
    titleLayout->setSpacing(0);
    titleLayout->setContentsMargins(0,0,0,0);

    m_pMenuLayout = new QHBoxLayout;
    m_pMenuLayout->setSpacing(0);
    m_pMenuLayout->setContentsMargins(0,0,0,0);


    QHBoxLayout *hbox = new QHBoxLayout;
    hbox->addLayout(titleLayout);
    hbox->addLayout(m_pMenuLayout);
    hbox->addStretch();
    hbox->addWidget(m_pMinButton);
    hbox->addWidget(m_pMaxButton);
    hbox->addWidget(m_pCloseButton);
    hbox->setSpacing(2);
    hbox->setContentsMargins(0,0,0,0);

    m_pTitleFrame->setLayout(hbox);

    m_bMaxNormal = true;

    connect(m_pCloseButton, SIGNAL( clicked() ), parent, SLOT(close() ) );
    connect(m_pMinButton, SIGNAL( clicked() ), this, SLOT(showSmall() ) );
    connect(m_pMaxButton, SIGNAL( clicked() ), this, SLOT(showMaxRestore() ) );

    QVBoxLayout *pVBox = new QVBoxLayout;
    pVBox->addWidget(m_pTitleFrame);
    pVBox->setContentsMargins(0,0,0,0);
    pVBox->setSpacing(0);
    setLayout(pVBox);
}

void BCTitleBar::slotClickAccount()
{
    emit sigClickAccount();
}

void BCTitleBar::showSmall()
{
    parentWidget()->showMinimized();
}

void BCTitleBar::showMaxRestore()
{
    if (m_bMaxNormal) {
        BCMainFrame *pFrame  = static_cast< BCMainFrame* >(parentWidget());
        pFrame->setGeometry(pFrame->restoreGeometry());

        m_bMaxNormal = !m_bMaxNormal;
    } else {
        QRect deskRect = QGuiApplication::screenAt(parentWidget()->geometry().center())->availableGeometry();
        parentWidget()->setGeometry(deskRect);

        m_bMaxNormal = !m_bMaxNormal;
    }
}

void BCTitleBar::mouseDoubleClickEvent(QMouseEvent *)
{
    if(!m_bResizeEnable)
    {
        return;
    }

    m_bMousePress = false;

    m_pMaxButton->click();
}

void BCTitleBar::mousePressEvent(QMouseEvent *)
{
    if(!m_bResizeEnable)
    {
        return;
    }

    m_bMousePress = true;

    m_PressPos = QCursor::pos();

    if(!m_bMaxNormal)
    {
        m_ClickPos = QCursor::pos()-parentWidget()->pos();

    }
}

void BCTitleBar::mouseMoveEvent(QMouseEvent *)
{
    if(!m_bResizeEnable)
    {
        return;
    }

    if(!m_bMousePress)
    {
        return;
    }

    QPoint tPoint = m_PressPos - QCursor::pos();
    // 需要移动一定距离才生效
    if(tPoint.manhattanLength() < (width() / 200.0f))
    {
        return;
    }

    BCMainFrame *pFrame  = static_cast< BCMainFrame* >(parentWidget());

    if (m_bMaxNormal)
    {
        pFrame->resize(pFrame->restoreSize());
        m_bMaxNormal = !m_bMaxNormal;
        int x   = QCursor::pos().x()-(float)parentWidget()->width()/2-parentWidget()->x();
        //获取可用桌面大小
        QRect deskRect = QGuiApplication::screenAt(parentWidget()->geometry().center())->availableGeometry();
        if(x < 0)
        {
            x   = 0;
        }else if(x+(float)parentWidget()->width() > deskRect.width())
        {
            x   = deskRect.width()-parentWidget()->width();
        }
        x   += parentWidget()->x();
        parentWidget()->move(x, 0);
        m_ClickPos  = QCursor::pos()-parentWidget()->pos();
    }
    else
    {
        if((QCursor::pos()-m_ClickPos).y() > 0)
        {
            parentWidget()->move(QCursor::pos()-m_ClickPos);
        }
        else
        {
            parentWidget()->move((QCursor::pos()-m_ClickPos).x(), 0);
        }
        pFrame->setRestoreGeometry(pFrame->geometry());
    }
}

void BCTitleBar::mouseReleaseEvent(QMouseEvent *)
{
    if(!m_bResizeEnable)
    {
        return;
    }

    m_bMousePress = false;

    if(QCursor::pos().y() <= 0 && parentWidget()->pos().y() <=0)
    {
        //获取可用桌面大小
        QRect deskRect = QGuiApplication::screenAt(parentWidget()->geometry().center())->availableGeometry();
        parentWidget()->setGeometry(deskRect);
        m_bMaxNormal = !m_bMaxNormal;
    }
}

void BCTitleBar::addMenuButton(QPushButton *pButton)
{
    m_MenuButtonList.append(pButton);
    m_pMenuLayout->addWidget(pButton);
}

void BCTitleBar::setResizeEnable(bool bEnable)
{
    m_bResizeEnable = bEnable;

    if(!m_bResizeEnable)
    {
        m_pMinButton->hide();
        m_pMaxButton->hide();
        m_bMaxNormal = false;
    }
    else
    {
        m_pMinButton->show();
        m_pMaxButton->show();
        m_bMaxNormal = true;
    }

}

/***************** StatusBar *****************/
BCStatusBar::BCStatusBar(QWidget *parent) :
    QWidget(parent)
{

    setAutoFillBackground(true);
    setPalette(QPalette(QColor("#171e24")));


//        m_pStatuLabel   = new QLabel(this);
//        m_pStatuLabel->setStyleSheet("color:white;");
//        m_pStatuLabel->setFont(QFont("微软雅黑", 10));

    m_pHLayout = new QHBoxLayout(this);
    QMargins margins(60, 0, 22, 0);
    m_pHLayout->setContentsMargins(margins);
    m_pHLayout->addStretch();
}

void BCStatusBar::setText(QString text)
{
//    m_pStatuLabel->setText(text);
}

void BCStatusBar::addWidget(QWidget *pWidget)
{
    if(m_pHLayout)
    {
        m_pHLayout->addWidget(pWidget);
    }
}

void BCStatusBar::insertWidget(int index, QWidget *pWidget)
{
    if(m_pHLayout)
    {
        m_pHLayout->insertWidget(index, pWidget);
    }
}

/***************** MainFrame *****************/
BCMainFrame::BCMainFrame(IPluginManager *pPluginManager) :
    m_pPluginManager(pPluginManager)
{
    m_BorderColor = QColor(FRAME_BORDER_COLOR);
    m_nBorderWidth = FRAME_BORDER_WIDTH;
    m_ContentMinimumSize = QSize(0, 0);
    m_pTitleBar = nullptr;
    m_pContent = nullptr;
    m_pStatusBar = nullptr;
    setObjectName("MainFrame");
    m_bMouseDown = false;
    setFrameShape(Panel);
    setStyleSheet("QFrame#MainFrame {border:none;}");
    setWindowFlags(Qt::FramelessWindowHint);
    setMouseTracking(true);

    startTimer(100);
    m_bSetCursor = false;
}

void BCMainFrame::CreateWidget()
{
    m_pTitleBar = new BCTitleBar(this);

    m_pContent = new BCMainWindow(m_pPluginManager);
    m_pContent->InitWidget();
    m_pContent->installEventFilter(this);

    m_pContent->setMouseTracking(true);

    QVBoxLayout *vbox = new QVBoxLayout(this);
    vbox->addWidget(m_pTitleBar);
    vbox->setContentsMargins(m_nBorderWidth,m_nBorderWidth,m_nBorderWidth,m_nBorderWidth);
    vbox->setSpacing(0);
    vbox->addWidget(m_pContent);

    m_pStatusBar = new BCStatusBar(this);
    vbox->addWidget(m_pStatusBar);
    m_pStatusBar->setHidden(true);
}

void BCMainFrame::setFlag(Qt::WindowFlags flag)
{

}

void BCMainFrame::paintEvent(QPaintEvent *pEvent)
{
    QPainter painter(this);
    painter.setPen(QPen(m_BorderColor, m_nBorderWidth, Qt::SolidLine));//设置画笔形式

    QVector<QPoint> pointPairs;
    pointPairs.append(QPoint(m_nBorderWidth/2, 0));
    pointPairs.append(QPoint(m_nBorderWidth/2, height()));

    pointPairs.append(QPoint(m_nBorderWidth*3/2, height()-m_nBorderWidth/2));
    pointPairs.append(QPoint(width()-m_nBorderWidth*3/2, height()-m_nBorderWidth/2));

    pointPairs.append(QPoint(width()-m_nBorderWidth/2, 0));
    pointPairs.append(QPoint(width()-m_nBorderWidth/2, height()));

    pointPairs.append(QPoint(m_nBorderWidth*3/2, m_nBorderWidth/2));
    pointPairs.append(QPoint(width()-m_nBorderWidth*3/2, m_nBorderWidth/2));


    painter.drawLines(pointPairs);

    QFrame::paintEvent(pEvent);
}

void BCMainFrame::changeEvent(QEvent *pEvent)
{
    if(pEvent->type() != QEvent::WindowStateChange)
        return;

    RETURN_IF_PTR_IS_NULL(m_pContent);

    m_pContent->setWindowState(windowState());
}

void BCMainFrame::showEvent(QShowEvent *pEvent)
{
    m_pContent->show();

    QFrame::showEvent(pEvent);
}

bool BCMainFrame::eventFilter(QObject *obj, QEvent *event)
{
    if(obj == m_pContent)
    {
        if(event->type() == QEvent::Close)
        {
            close();
        }
        else if (event->type() == QEvent::Wheel)
        {
            int t = 0;
        } 
    }
    return false;
}

void BCMainFrame::mousePressEvent(QMouseEvent *e)
{
    if(m_pTitleBar)
    {
        if(!m_pTitleBar->resizeEnable())
        {
            return;
        }
    }

    m_OldPos = e->pos();
    m_bMouseDown = e->button() == Qt::LeftButton;
}

void BCMainFrame::mouseMoveEvent(QMouseEvent *e)
{
    if(m_pTitleBar)
    {
        if(!m_pTitleBar->resizeEnable())
        {
            return;
        }
    }

    int x = e->x();
    int y = e->y();

    if (m_bMouseDown)
    {
        int dx = x - m_OldPos.x();
        int dy = y - m_OldPos.y();

        QRect g = geometry();

        if (m_bLeft)
        {
            g.setLeft(g.left() + dx);
        }
        if (m_bRight)
        {
            g.setRight(g.right() + dx);
        }
        if (m_bBottom)
        {
            g.setBottom(g.bottom() + dy);
        }
        if(m_bTop)
        {
            g.setTop(g.top() + dy);
        }

        if(g.width() < m_ContentMinimumSize.width() || g.height() < m_ContentMinimumSize.height())
        {
            return;
        }

        setGeometry(g);

        m_RestoreGeometry    = g;
        m_OldPos = QPoint(!m_bLeft ? e->x() : m_OldPos.x(), !m_bTop ? e->y() : m_OldPos.y());
    }
}

void BCMainFrame::mouseReleaseEvent(QMouseEvent *)
{
    if(m_pTitleBar)
    {
        if(!m_pTitleBar->resizeEnable())
        {
            return;
        }
    }

    m_bMouseDown = false;
}

void BCMainFrame::timerEvent(QTimerEvent *)
{
    if(m_pTitleBar)
    {
        if(!m_pTitleBar->resizeEnable())
        {
            return;
        }
    }

    if(!m_bMouseDown)
    {

        int x = QCursor::pos().x();
        int y = QCursor::pos().y();

        QRect r = rect();

        m_bLeft = (x - r.left()-this->x()) < m_nBorderWidth;
        m_bRight = (r.right()+this->x()-x) < m_nBorderWidth;
        m_bBottom = (r.bottom()+this->y()-y) < m_nBorderWidth;
        m_bTop  = (y-r.top()-this->y()) < m_nBorderWidth;

        bool hor = m_bLeft | m_bRight;

        if (hor && m_bBottom)
        {
            if (m_bLeft)
            {
                setCursor(Qt::SizeBDiagCursor);
            }
            else
            {
                setCursor(Qt::SizeFDiagCursor);
            }
            m_bSetCursor = true;
        }
        else if (hor)
        {
            setCursor(Qt::SizeHorCursor);
            m_bSetCursor = true;
        }
        else if (m_bBottom | m_bTop)
        {
            setCursor(Qt::SizeVerCursor);
            m_bSetCursor = true;
        }
        else
        {
            if(m_bSetCursor)
            {
                setCursor(Qt::ArrowCursor);
                m_bSetCursor = false;
            }

        }
    }
}

void BCMainFrame::closeEvent(QCloseEvent *pEvent)
{
    if(!m_pContent->close())
    {
        pEvent->ignore();
    }
}

void BCMainFrame::resizeEvent(QResizeEvent *pEvent)
{

    QFrame::resizeEvent(pEvent);
}

void BCMainFrame::moveEvent(QMoveEvent *pEvent)
{

    QFrame::moveEvent(pEvent);
}

void BCMainFrame::close()
{
    QCoreApplication::instance()->exit();
}
