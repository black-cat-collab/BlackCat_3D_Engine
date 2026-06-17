#include "tcpserver.h"
#include "servermanager.h"
#include "websockettransfer.h"

#ifdef _WIN32
#include "WinUser.h"
#endif


using namespace bc;

SenderTcpServer::SenderTcpServer(QObject *parent) :
    QTcpServer(parent)
{

}

SenderTcpServer::~SenderTcpServer()
{

}

void SenderTcpServer::incomingConnection(qintptr handle)
{
    emit sigNewConnection(handle);
}


SenderThread::SenderThread(ISystemAPI *pSystemAPI) :
    m_pSystemAPI(pSystemAPI),
    m_pTcpServer(NULL),
    m_pTcpClient(NULL),
    m_pSocketDescriptor(-1)
{
    m_nLocalPort = 8808;

    m_pTcpServer = new SenderTcpServer;

    connect(m_pTcpServer, SIGNAL(sigNewConnection(qintptr)), this, SLOT(slotNewConnection(qintptr)));
    //m_pTcpServer->listen(QHostAddress::Any, m_nLocalPort);
}

SenderThread::~SenderThread()
{
    requestInterruption();

    wait();

    if(m_pTcpServer)
    {
        m_pTcpServer->close();
        m_pTcpServer->deleteLater();
    }


}


void SenderThread::slotNewConnection(qintptr handle)
{
    {
        QMutexLocker locker(&m_DescriptorMutex);
        m_pSocketDescriptor = handle;
    }


    qDebug()<<"new connection ********";
}

void SenderThread::slotSetClient(const QString &strClientIP, int nLocalPort)
{
    m_nLocalPort = nLocalPort;
    if(m_pTcpServer->isListening())
    {
        m_pTcpServer->close();
    }

    qDebug()<<"listen ********";
    m_pTcpServer->listen(QHostAddress::Any, m_nLocalPort);
}

void SenderThread::SetSendData(const QByteArray &data)
{
    if(!isRunning())
    {
        start();
    }

    if(m_pTcpClient == NULL)
    {
        return;
    }

    if(m_DataMutex.tryLock())
    {
        m_SendData = data;
        m_DataMutex.unlock();
    }
}



void SenderThread::slotDisconnected()
{
    ServerManager::GetInstance()->GetTransfer()->stopSending(false);

}

void SenderThread::run()
{
    while(!isInterruptionRequested())
    {
        if(m_pTcpClient == NULL)
        {
            m_pTcpClient = new QTcpSocket;
            m_pTcpClient->setSocketOption(QAbstractSocket::LowDelayOption, 1);
            connect(m_pTcpClient, SIGNAL(disconnected()), this, SLOT(slotDisconnected()));
        }

        {
            QMutexLocker locker(&m_DescriptorMutex);
            if(m_pSocketDescriptor != m_pTcpClient->socketDescriptor() &&
               m_pSocketDescriptor != -1)
            {
                m_pTcpClient->setSocketDescriptor(m_pSocketDescriptor);
            }
        }

        if(m_pTcpClient->state() == QAbstractSocket::UnconnectedState)
        {
            continue;
        }

        if(m_SendData.isEmpty())
        {
            continue;
        }

        if(m_DataMutex.tryLock())
        {
            int nWrite = m_pTcpClient->write(m_SendData.data(), m_SendData.size());

            if(nWrite < 0)
            {
                continue;
            }

            m_pTcpClient->waitForBytesWritten();

            m_SendData.clear();

            m_DataMutex.unlock();
        }
    }
}
