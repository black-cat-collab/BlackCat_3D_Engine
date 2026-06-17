#include "BlackCat/bcmessagebox.h"

BCMessageBox::BCMessageBox( QString text, QString title, QWidget *parent) :
    QDialog(parent),
    m_bMoving(false),
    m_bMoveEnable(false),
    m_pTitleLabel(nullptr),
    m_pTextLabel(nullptr),
    m_pWidgetLayout(nullptr),
    m_pButtonLayout(nullptr),
    m_pClickedButton(nullptr),
    m_strObjectName("BCMessageBoxFrame")
{
    init();

    m_pTextLabel->setText(text);
    m_pTitleLabel->setText(title.isEmpty() ? QStringLiteral("提示") : title);

    setMouseTracking(true);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
}

BCMessageBox::~BCMessageBox()
{

}

void BCMessageBox::init()
{
    QVBoxLayout *pVLayout = new QVBoxLayout;
    pVLayout->setSpacing(0);
    pVLayout->setContentsMargins(0,0,0,0);

    QFrame *pFrame = new QFrame(this);
    pFrame->setObjectName(m_strObjectName);
    pFrame->setStyleSheet(QString("QFrame#%1{border:1px solid rgb(52, 52, 52);}").arg(m_strObjectName));
    pVLayout->addWidget(pFrame);

    setLayout(pVLayout);

    m_pWidgetLayout = new QVBoxLayout;
    pFrame->setLayout(m_pWidgetLayout);
    m_pWidgetLayout->setContentsMargins(0,0,0,0);
    m_pWidgetLayout->setSpacing(10);

    m_pTitleLabel   = new QLabel(pFrame);
    m_pTitleLabel->setFont(QFont(QStringLiteral("微软雅黑"), 12));
    m_pTitleLabel->setFixedHeight(30);
    m_pWidgetLayout->addWidget(m_pTitleLabel);

    m_pTextLabel   = new QLabel(pFrame);
    m_pTextLabel->setFont(QFont(QStringLiteral("微软雅黑"), 10));
    m_pTextLabel->setFixedHeight(100);
    m_pWidgetLayout->addWidget(m_pTextLabel);

    QFrame* pButtonFrame = new QFrame(pFrame);
    pButtonFrame->setObjectName("ButtonFrame");
    pButtonFrame->setStyleSheet("QFrame#ButtonFrame{border:none;}");

    m_pButtonLayout = new QHBoxLayout;
    m_pButtonLayout->setSpacing(20);
    m_pButtonLayout->setContentsMargins(0,0,0,0);
    QHBoxLayout* pHLayout = new QHBoxLayout;
    pHLayout->addStretch();
    pHLayout->addLayout(m_pButtonLayout);
    pHLayout->addStretch();

    pButtonFrame->setLayout(pHLayout);
    m_pWidgetLayout->addWidget(pButtonFrame);

    resize(410, 200);
}

QPushButton* BCMessageBox::addButton(QString text)
{
    QPushButton *pButton    = new QPushButton(this);
    pButton->setText(text);
    pButton->setFont(QFont(QStringLiteral("微软雅黑")));
    m_pButtonLayout->addWidget(pButton);
    connect(pButton, SIGNAL(clicked()), this, SLOT(slotClose()));

    return pButton;
}

void BCMessageBox::slotClose()
{
    QPushButton *pButton    = qobject_cast<QPushButton*>(sender());
    m_pClickedButton    = pButton;
    close();
}

void BCMessageBox::mousePressEvent(QMouseEvent* event)
{
    if(event->button() == Qt::LeftButton && m_bMoveEnable)
    {
        m_bMoving = true;
        m_LastMousePosition = event->globalPos();
    }
}

void BCMessageBox::mouseMoveEvent(QMouseEvent* event)
{
    if(event->buttons().testFlag(Qt::LeftButton) && m_bMoving && m_bMoveEnable && !m_LastMousePosition.isNull())
    {
        this->move(this->pos() + (event->globalPos() - m_LastMousePosition));
        m_LastMousePosition = event->globalPos();
    }
}

void BCMessageBox::mouseReleaseEvent(QMouseEvent* event)
{
    if(event->button() == Qt::LeftButton)
    {
        m_bMoving = false;
    }
}

