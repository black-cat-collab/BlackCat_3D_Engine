/********************************************************************************
** Form generated from reading UI file 'logindialog.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOGINDIALOG_H
#define UI_LOGINDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFrame>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_LoginDialog
{
public:
    QVBoxLayout *verticalLayout;
    QFrame *backgroundFrame;
    QPushButton *loginButton;
    QLabel *label;
    QLineEdit *accountEdit;
    QLineEdit *passwordEdit;
    QPushButton *closeButton;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *label_4;
    QLabel *label_5;
    QLabel *loginTips_0;
    QLabel *loginTips_1;
    QLabel *loginTips_2;

    void setupUi(QDialog *LoginDialog)
    {
        if (LoginDialog->objectName().isEmpty())
            LoginDialog->setObjectName(QString::fromUtf8("LoginDialog"));
        LoginDialog->resize(1920, 1080);
        verticalLayout = new QVBoxLayout(LoginDialog);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        backgroundFrame = new QFrame(LoginDialog);
        backgroundFrame->setObjectName(QString::fromUtf8("backgroundFrame"));
        backgroundFrame->setStyleSheet(QString::fromUtf8("QFrame#backgroundFrame{border-image: url(:/resource/image/login_bg.png);}\n"
""));
        backgroundFrame->setFrameShape(QFrame::StyledPanel);
        backgroundFrame->setFrameShadow(QFrame::Raised);
        loginButton = new QPushButton(backgroundFrame);
        loginButton->setObjectName(QString::fromUtf8("loginButton"));
        loginButton->setGeometry(QRect(850, 680, 210, 52));
        QFont font;
        font.setFamily(QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221"));
        font.setPointSize(12);
        loginButton->setFont(font);
        loginButton->setStyleSheet(QString::fromUtf8("QPushButton{border-image: url(:/resource/image/btn_login_normal.png);}\n"
"QPushButton:hover{border-image: url(:/resource/image/btn_login_selected.png);}\n"
"QPushButton:pressed{border-image: url(:/resource/image/btn_login_selected.png);}\n"
""));
        label = new QLabel(backgroundFrame);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(40, 40, 601, 85));
        QFont font1;
        font1.setFamily(QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221"));
        font1.setPointSize(20);
        label->setFont(font1);
        label->setStyleSheet(QString::fromUtf8("border-image: url(:/resource/image/Logo.png);"));
        label->setAlignment(Qt::AlignCenter);
        accountEdit = new QLineEdit(backgroundFrame);
        accountEdit->setObjectName(QString::fromUtf8("accountEdit"));
        accountEdit->setGeometry(QRect(720, 429, 499, 73));
        QFont font2;
        font2.setFamily(QString::fromUtf8("Source Han Sans CN"));
        font2.setBold(false);
        font2.setWeight(50);
        accountEdit->setFont(font2);
        accountEdit->setStyleSheet(QString::fromUtf8("QLineEdit{border-image: url(:/resource/image/login_input_normal.png);}\n"
"\n"
"\n"
"QLineEdit{\n"
"font-size:24px;\n"
"font-family:Source Han Sans CN;\n"
"font-weight:400;\n"
"color:rgba(255,255,255,1);}\n"
"\n"
""));
        accountEdit->setCursorPosition(0);
        accountEdit->setAlignment(Qt::AlignCenter);
        accountEdit->setCursorMoveStyle(Qt::VisualMoveStyle);
        passwordEdit = new QLineEdit(backgroundFrame);
        passwordEdit->setObjectName(QString::fromUtf8("passwordEdit"));
        passwordEdit->setGeometry(QRect(720, 561, 499, 73));
        passwordEdit->setFont(font2);
        passwordEdit->setStyleSheet(QString::fromUtf8("QLineEdit{color:#8abcc3;border:1px solid #3e9eb3;background:transparent;}\n"
"QLineEdit:hover{border:1px solid #0ff5f5;}\n"
"\n"
"QLineEdit{border-image: url(:/resource/image/login_input_normal.png);}\n"
"QLineEdit:pressed{border-image: url(:/resource/image/login_input_selected.png);}\n"
"\n"
"QLineEdit{\n"
"font-size:24px;\n"
"font-family:Source Han Sans CN;\n"
"font-weight:400;\n"
"color:rgba(255,255,255,1);}"));
        passwordEdit->setEchoMode(QLineEdit::Password);
        passwordEdit->setAlignment(Qt::AlignCenter);
        closeButton = new QPushButton(backgroundFrame);
        closeButton->setObjectName(QString::fromUtf8("closeButton"));
        closeButton->setGeometry(QRect(1809, 57, 55, 55));
        closeButton->setFocusPolicy(Qt::NoFocus);
        closeButton->setStyleSheet(QString::fromUtf8("QPushButton{border-image: url(:/resource/image/btn_close_normal.png);}\n"
"QPushButton:hover{border-image: url(:/resource/image/btn_close_press.png);}\n"
"QPushButton:pressed{border-image: url(:/resource/image/btn_close_press.png);}\n"
"\n"
""));
        label_2 = new QLabel(backgroundFrame);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(610, 300, 710, 535));
        label_2->setStyleSheet(QString::fromUtf8("border-image: url(:/resource/image/login_border.png);"));
        label_3 = new QLabel(backgroundFrame);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(720, 384, 65, 26));
        label_3->setStyleSheet(QString::fromUtf8("width:65px;\n"
"height:26px;\n"
"font-size:28px;\n"
"font-family:Source Han Sans CN;\n"
"font-weight:400;\n"
"color:rgba(255,255,255,1);"));
        label_4 = new QLabel(backgroundFrame);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(720, 515, 65, 26));
        label_4->setStyleSheet(QString::fromUtf8("width:65px;\n"
"height:26px;\n"
"font-size:28px;\n"
"font-family:Source Han Sans CN;\n"
"font-weight:400;\n"
"color:rgba(255,255,255,1);"));
        label_5 = new QLabel(backgroundFrame);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(790, 994, 501, 27));
        label_5->setStyleSheet(QString::fromUtf8("width:501px;\n"
"height:27px;\n"
"font-size:28px;\n"
"font-family:Source Han Sans CN;\n"
"font-weight:400;\n"
"color:rgba(255,255,255,1);\n"
"opacity:0.8;"));
        loginTips_0 = new QLabel(backgroundFrame);
        loginTips_0->setObjectName(QString::fromUtf8("loginTips_0"));
        loginTips_0->setGeometry(QRect(860, 640, 321, 20));
        QFont font3;
        font3.setFamily(QString::fromUtf8("Source Han Sans CN"));
        loginTips_0->setFont(font3);
        loginTips_0->setStyleSheet(QString::fromUtf8("font-size:20px;\n"
"font-family:Source Han Sans CN;\n"
"color:rgba(255,69,69,1);"));
        loginTips_1 = new QLabel(backgroundFrame);
        loginTips_1->setObjectName(QString::fromUtf8("loginTips_1"));
        loginTips_1->setGeometry(QRect(1034, 384, 171, 19));
        loginTips_1->setFont(font3);
        loginTips_1->setStyleSheet(QString::fromUtf8("\n"
"font-size:20px;\n"
"font-family:Source Han Sans CN;\n"
"color:rgba(255,69,69,1);"));
        loginTips_2 = new QLabel(backgroundFrame);
        loginTips_2->setObjectName(QString::fromUtf8("loginTips_2"));
        loginTips_2->setGeometry(QRect(1034, 515, 171, 19));
        loginTips_2->setFont(font3);
        loginTips_2->setStyleSheet(QString::fromUtf8("font-size:20px;\n"
"font-family:Source Han Sans CN;\n"
"color:rgba(255,69,69,1);"));
        label_2->raise();
        loginButton->raise();
        label->raise();
        accountEdit->raise();
        passwordEdit->raise();
        closeButton->raise();
        label_3->raise();
        label_4->raise();
        label_5->raise();
        loginTips_0->raise();
        loginTips_1->raise();
        loginTips_2->raise();

        verticalLayout->addWidget(backgroundFrame);

        QWidget::setTabOrder(accountEdit, passwordEdit);
        QWidget::setTabOrder(passwordEdit, loginButton);

        retranslateUi(LoginDialog);

        QMetaObject::connectSlotsByName(LoginDialog);
    } // setupUi

    void retranslateUi(QDialog *LoginDialog)
    {
        LoginDialog->setWindowTitle(QCoreApplication::translate("LoginDialog", "Dialog", nullptr));
        loginButton->setText(QString());
        label->setText(QString());
        accountEdit->setText(QString());
        accountEdit->setPlaceholderText(QString());
        passwordEdit->setText(QString());
        passwordEdit->setPlaceholderText(QString());
        closeButton->setText(QString());
        label_2->setText(QString());
        label_3->setText(QCoreApplication::translate("LoginDialog", "\350\264\246\345\217\267\357\274\232", nullptr));
        label_4->setText(QCoreApplication::translate("LoginDialog", "\345\257\206\347\240\201\357\274\232", nullptr));
        label_5->setText(QCoreApplication::translate("LoginDialog", "<html><head/><body><p>\347\211\210\346\235\203\346\211\200\346\234\211\357\274\232\346\267\261\345\234\263\345\270\202\351\273\221\347\214\253\347\247\221\346\212\200\346\234\211\351\231\220\345\205\254\345\217\270</p></body></html>", nullptr));
        loginTips_0->setText(QCoreApplication::translate("LoginDialog", "\347\224\250\346\210\267\345\220\215\344\270\216\345\257\206\347\240\201\344\270\215\345\214\271\351\205\215\357\274\201", nullptr));
        loginTips_1->setText(QCoreApplication::translate("LoginDialog", "\346\202\250\350\276\223\345\205\245\347\232\204\350\264\246\345\217\267\346\234\211\350\257\257\357\274\201", nullptr));
        loginTips_2->setText(QCoreApplication::translate("LoginDialog", "\346\202\250\350\276\223\345\205\245\347\232\204\345\257\206\347\240\201\346\234\211\350\257\257\357\274\201", nullptr));
    } // retranslateUi

};

namespace Ui {
    class LoginDialog: public Ui_LoginDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOGINDIALOG_H
