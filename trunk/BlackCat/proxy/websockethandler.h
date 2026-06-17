#ifndef WEBSOCKETHANDLER_H
#define WEBSOCKETHANDLER_H

#include <QObject>
#include <QDebug>
#include <QTimer>
#include <QtWebSockets/qwebsocket.h>
#include <QSize>
#include <QMap>
#include <QJsonValue>

#include "BlackCat/corecommon.h"



enum AuthValue
{
    AUTH_NONE=0,
    AUTH_SUCCESS,
    AUTH_TWO_TIMES
};

class WebSocketHandler : public QObject
{
    Q_OBJECT
public:
    explicit WebSocketHandler(QString strName,QObject *parent = Q_NULLPTR);

    enum WaitResponseType
    {
        WaitTypeUnknown,
        WaitTypeRemoteAuth
    };

private:
    QWebSocket          *  m_webSocket;
    QTimer              *  m_timerReconnect;
    QTimer              *  m_timerWaitResponse;
    QTimer              *  m_timerHeartbeat;


    int        m_waitType;
    quint16    m_nAuthenticated;
    QString    m_url;
    QString    m_name;

    QString    m_login;
    QString    m_pass;

    QByteArray m_dataTmp;
    QByteArray m_uuid;
    QByteArray m_nonce;

    int         m_nLastTime;


signals:
    void finished();
    void getDesktop();
    void connectedStatus(bool);
    void authenticatedStatus(bool);

    void setKeyPressed(quint16 keyCode, bool state);
    void setMousePressed(quint16 keyCode, bool state);
    void setWheelChanged(bool deltaPos);
    void setScaleSize(quint16 posX, quint16 posY);
    void setMouseMove(quint16 posX, quint16 posY);
    void setMouseDelta(qint16 deltaX, qint16 deltaY);
    void disconnected(WebSocketHandler *pointer);
    void disconnectedUuid(const QByteArray &uuid);
    void remoteAuthenticationRequest(const QByteArray &uuid, const QByteArray &nonce, const QByteArray &request);
    void remoteAuthenticationResponse(const QByteArray &uuidDst, const QByteArray &uuidSrc,
                                      const QByteArray &name, bool state);

    void appDataInterface(const QByteArray &data);
    void sigSendJsData(QString strData);
    void sigSetClient(const QString &strHost,int nPort);
    void sigCmdDisconnected(WebSocketHandler *pCmdWebSocketHandler);


public slots:
    void createSocket();
    void removeSocket();
    void setUrl(const QString &url);


    void setName(const QString &name);
    QString getName();

    QByteArray getUuid();

    void setLoginPass();

    void setCmdSocket(QWebSocket *webSocket,bool bAutoLogin=false);
    void setDataSocket(QWebSocket *webSocket);
    void setAppSocket(QWebSocket *webSocket);

    void setCallback(bool bSet=true);


    QWebSocket *getSocket();


    void sendImageParameters(const QSize &imageSize, int rectWidth);
    void sendImageData(const QByteArray &imageData);

     static int  sendJsData(INT_PTR wParam,INT_PTR lParam);


    void sendAppData(char *pData,int nLength);
    void sendAppBinary(const QByteArray &appData);
    void sendAppText(QString strData);



    void sendBinaryMessage(const QByteArray &data);


    void sendName(const QString &name);
    void checkRemoteAuthentication(const QByteArray &uuid, const QByteArray &nonce, const QByteArray &request);
    void setRemoteAuthenticationResponse(const QByteArray &uuid, const QByteArray &name);

    void slotWebrtcFailed();

    

private slots:
    void newData(const QByteArray &command, const QByteArray &data);

    void sendAuthenticationResponse(quint16 state);
    void sendRemoteAuthenticationResponse(const QByteArray &uuid, const QByteArray &nonce, const QByteArray &request);
    QByteArray getHashSum(const QByteArray &nonce, const QString &login, const QString &pass);
    void socketStateChanged(QAbstractSocket::SocketState state);
    void socketDisconnected();

    void textMessageReceived(const QString &message);
    void binaryMessageReceived(const QByteArray &data);
    void timerReconnectTick();
	void timerHeartbeat();

    void startWaitResponseTimer(int msec, int type);
    void stopWaitResponseTimer();
    void timerWaitResponseTick();

    void debugHexData(const QByteArray &data);
    void doLoginSuccess();
    void getLoginInfo(const QByteArray &data, QString &strUser,QString &strPass);

public:
    static QByteArray arrayFromUint16(quint16 number);
    static quint16 uint16FromArray(const QByteArray &buf);
};

#endif // WEBSOCKETHANDLER_H
