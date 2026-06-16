#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QMouseEvent>
#include <loginplugin.h>
#include <QTimer>

namespace Ui {
class LoginDialog;
class QTimer;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = 0);
    ~LoginDialog();

    static QString m_strUsername;
    static QString m_strPassword;
    static bool    m_bLoginState;

protected:
    bool eventFilter(QObject *obj, QEvent *event);
private:
    Ui::LoginDialog *ui;
    QPoint mLastMousePosition;
    bool mMoving;

    QTimer* m_pTimer;

private:
    void init();

    void Timer();

public slots:
    void slotLogin();
    void slotHideLable();
};

#endif // LOGINDIALOG_H
