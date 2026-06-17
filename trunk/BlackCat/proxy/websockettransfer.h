#ifndef WEBSOCKETTRANSFER_H
#define WEBSOCKETTRANSFER_H

#include <QObject>
#include <QDebug>
#include <QtWebSockets/qwebsocketserver.h>
#include <QtWebSockets/qwebsocket.h>
#include "websockethandler.h"
#include "BlackCat/corecommon.h"


class WebSocketTransfer : public QObject
{
    Q_OBJECT
public:
    explicit WebSocketTransfer(QObject *parent = Q_NULLPTR);



private:


    bool     m_bAutoLogin;
    int      m_nAuthStatus;


    QList<WebSocketHandler*> m_cmdSockets;          //鼠标操作等指令socket
    QWebSocketServer        *m_webCmdServer;

    QList<WebSocketHandler*> m_dataSockets;         //视频数据socket
    QWebSocketServer        *m_webDataServer;

    QWebSocketServer        *m_webAppServer;
    WebSocketHandler        *m_pSocketHandler;      //业务数据交互socket

    static void static_Cloud_handle_func(const std::string& strCode,QList<QJsonValue> list);

signals:
    void finished();
    void newCmdSocketConnected(WebSocketHandler *webSocket);
    void newDataSocketConnected(WebSocketHandler *webSocket);
    void newAppSocketConnected(WebSocketHandler *webSocket);
    void businessDisconnected();
    void sigWebRtcFailed();


public slots:
    void start();
    void stop();

    void setAutoLogin(QString strLogin);
    void setAuthStatus(quint16 nAuthStatus);

    void stopSending(bool bReset);


    quint16 getAuthStatus(){return m_nAuthStatus;};


    void checkRemoteAuthentication(const QByteArray &uuid, const QByteArray &nonce, const QByteArray &request);
    void setRemoteAuthenticationResponse(const QByteArray &uuidDst, const QByteArray &uuidSrc, const QByteArray &nameSrc);

    void slotReset();

    void clearSocket();
    void slotBusinessDisconnected();
    void slotStartClient();

private slots:
    void setCmdSocketConnected();
    void setDataSocketConnected();
    void setAppSocketConnected();




    void clearCallback();
};

WebSocketTransfer* GetTransfer();


#endif // WEBSOCKETTRANSFER_H
