#include "BlackCat/widget/docktitlewidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>

DockTitleWidget::DockTitleWidget(QString title, QWidget *parent) :
    QWidget(parent)
{
    init();

    m_pTitleLabel->setText(title);
}

DockTitleWidget::~DockTitleWidget()
{
}

void DockTitleWidget::init()
{
    QVBoxLayout *pVLayout = new QVBoxLayout;
    pVLayout->setSpacing(0);
    pVLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(pVLayout);

    m_pBackgroundFrame = new QFrame(this);
    pVLayout->addWidget(m_pBackgroundFrame);

    QHBoxLayout *pHLayout = new QHBoxLayout;
    pHLayout->setSpacing(0);
    pHLayout->setContentsMargins(0, 0, 0, 0);
    m_pBackgroundFrame->setLayout(pHLayout);

    m_pTitleFrame = new QFrame(this);
    pHLayout->addWidget(m_pTitleFrame);
    pHLayout->addStretch();

    QHBoxLayout *pTitleLayout = new QHBoxLayout;
    pTitleLayout->setSpacing(0);
    pTitleLayout->setContentsMargins(0, 0, 0, 0);
    m_pTitleFrame->setLayout(pTitleLayout);

    m_pTitleLabel = new QLabel(this);
    m_pCloseButton = new QPushButton(this);

    pTitleLayout->addWidget(m_pTitleLabel);
    pTitleLayout->addWidget(m_pCloseButton);

    connect(m_pCloseButton, &QPushButton::clicked, [this]()
    {
        emit sigClose();
    });
}
