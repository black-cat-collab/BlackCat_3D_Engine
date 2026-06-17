#ifndef __TCPSERVER_H__
#define __TCPSERVER_H__


#include <QMutex>
#include <QUdpSocket>
#include <QTcpServer>
#include <QThread>

#include "BlackCat/corecommon.h"

using namespace bc;

class SenderTcpServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit SenderTcpServer(QObject *parent = nullptr);
    virtual ~SenderTcpServer();
protected:
    virtual void incomingConnection(qintptr handle);
signals:
    void sigNewConnection(qintptr handle);
};

class SenderThread : public QThread
{
    Q_OBJECT
public:
    explicit SenderThread(ISystemAPI* pSystemAPI);
    virtual ~SenderThread();
    void SetSendData(const QByteArray& data);
protected:
    virtual void run();
private:
    ISystemAPI* m_pSystemAPI;
    SenderTcpServer* m_pTcpServer;
    QTcpSocket* m_pTcpClient;
    QMutex      m_DescriptorMutex;
    qintptr     m_pSocketDescriptor;
    QMutex      m_DataMutex;
    QByteArray  m_SendData;
    int         m_nLocalPort;
public slots:
    void slotNewConnection(qintptr handle);
    void slotDisconnected();

    void slotSetClient(const QString &strClientIP, int nLocalPort);

};

//namespace hcclouderend {
//    class HCCloudeRender;
//}

//class FrameEncoder;
//class RemoteDesktopUniting;

#endif   //__GLWIDGET_H__
