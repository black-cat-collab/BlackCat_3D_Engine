#include "logindialog.h"
#include "ui_logindialog.h"
#include <QTime>
#include <QDebug>

QString LoginDialog::m_strUsername = "";
QString LoginDialog::m_strPassword = "";
bool    LoginDialog::m_bLoginState = false;

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    init();
}

LoginDialog::~LoginDialog()
{
    delete ui;
}


void LoginDialog::init()
{
    m_pTimer = new QTimer(this);
    connect( m_pTimer, SIGNAL(timeout()), this, SLOT(slotHideLable()) );

    ui->loginTips_0->hide();
    ui->loginTips_1->hide();
    ui->loginTips_2->hide();

    setWindowFlags(Qt::FramelessWindowHint);
    setMouseTracking( true );

    ui->backgroundFrame->installEventFilter(this);
    connect(ui->loginButton, SIGNAL(clicked()), this, SLOT(slotLogin()));
    connect(ui->closeButton, SIGNAL(clicked()), this, SLOT(reject()));

    ui->accountEdit->installEventFilter(this);
    ui->passwordEdit->installEventFilter(this);
}

void LoginDialog::Timer()
{
    m_pTimer->stop();
    m_pTimer->start(2000);
    m_pTimer->setSingleShot(true);
}

bool LoginDialog::eventFilter(QObject *obj, QEvent *event)
{
    if(obj == ui->backgroundFrame)
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent *pMouseEvent    = static_cast<QMouseEvent*>(event);
            if(pMouseEvent->button() == Qt::LeftButton)
            {
                mMoving = true;
                mLastMousePosition = pMouseEvent->globalPos();
                return true;
            }
        }
        else if(event->type() == QEvent::MouseButtonRelease)
        {
            QMouseEvent *pMouseEvent    = static_cast<QMouseEvent*>(event);
            if(pMouseEvent->button() == Qt::LeftButton)
            {
                mMoving = false;
                return true;
            }
        }
        else if(event->type() == QEvent::MouseMove)
        {
            QMouseEvent *pMouseEvent    = static_cast<QMouseEvent*>(event);
            if( mMoving && !mLastMousePosition.isNull())
            {
                move(pos() + (pMouseEvent->globalPos() - mLastMousePosition));
                mLastMousePosition = pMouseEvent->globalPos();
                return true;
            }
        }
    }

    if(event->type() == QEvent::FocusIn)
    {
        if(obj == ui->accountEdit)
        {
            ui->accountEdit->setStyleSheet("QLineEdit{border-image: url(:/resource/image/login_input_selected.png);font-size:24px;color:rgba(255,255,255,1);}");
            //return true;
        }
        else if(obj == ui->passwordEdit)
        {
            ui->passwordEdit->setStyleSheet("QLineEdit{border-image: url(:/resource/image/login_input_selected.png);font-size:24px;color:rgba(255,255,255,1);}");
            //return true;
        }
    }

    if(event->type() == QEvent::FocusOut)
    {
        if(obj == ui->accountEdit)
        {
            ui->accountEdit->setStyleSheet("QLineEdit{border-image: url(:/resource/image/login_input_normal.png);font-size:24px;color:rgba(255,255,255,1);}");
        }
        else if(obj == ui->passwordEdit)
        {
            ui->passwordEdit->setStyleSheet("QLineEdit{border-image: url(:/resource/image/login_input_normal.png);font-size:24px;color:rgba(255,255,255,1);}");
        }
    }


    return false;
}

void LoginDialog::slotLogin()
{
    accept();
}

void LoginDialog::slotHideLable()
{
    ui->loginTips_0->hide();
    ui->loginTips_1->hide();
    ui->loginTips_2->hide();
}
