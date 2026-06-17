#include "websockettransfer.h"
#include "servermanager.h"
#include <QWebSocketCorsAuthenticator>
//#include "HCClouderender.h"

#include <QSslKey>

static WebSocketTransfer* g_pThis=NULL;

WebSocketTransfer* GetTransfer()
{
    return g_pThis;
}

WebSocketTransfer::WebSocketTransfer(QObject *parent) : QObject(parent),
    m_webCmdServer(Q_NULLPTR),
    m_bAutoLogin(false),
    m_nAuthStatus(0)
{
    g_pThis=this;

    //bc::HCCloudeRender::GetInstance()->AddQtCoreHandleFunc(static_Cloud_handle_func);
}

void WebSocketTransfer::start()
{
    int nCmdPort=0;

    if(m_webCmdServer)
        return;

    bool bHttps = (ServerManager::GetInstance()->GetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.strServerIp.find("https") != std::string::npos);
    m_webCmdServer = new QWebSocketServer("Cmd Socket Server",bHttps?QWebSocketServer::SecureMode:QWebSocketServer::NonSecureMode,this);
    m_webDataServer= new QWebSocketServer("Data Socket Server",bHttps?QWebSocketServer::SecureMode:QWebSocketServer::NonSecureMode,this);
    m_webAppServer= new QWebSocketServer("App Socket Server",bHttps?QWebSocketServer::SecureMode:QWebSocketServer::NonSecureMode,this);

    if(ServerManager::GetInstance()->GetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.strServerIp.find("https") != std::string::npos)
    {
        QList<QWebSocketServer*> list;
        list << m_webCmdServer << m_webDataServer << m_webAppServer;
        for(int i = 0 ; i < list.size() ; i++)
        {
            QString strCrt = QApplication::applicationDirPath() + "/server.crt";
            QString strKey = QApplication::applicationDirPath() + "/server.key";
            QFile certFile(strCrt);
            QFile keyFile(strKey);
            if(!certFile.open(QIODevice::ReadOnly)){
                qDebug() << "certfile open failed...";
            }
            if(!keyFile.open(QIODevice::ReadOnly)){
                qDebug() << "keyfile open failed...";
            }
            QSslCertificate cert(&certFile, QSsl::Pem);
            QSslKey key(&keyFile, QSsl::Rsa, QSsl::Pem);
            certFile.close();
            keyFile.close();

            QSslConfiguration conf = list[i]->sslConfiguration();

            conf.setLocalCertificate(cert);
            conf.setPrivateKey(key);

            conf.setPeerVerifyMode(QSslSocket::VerifyNone);
            conf.setProtocol(QSsl::TlsV1_2OrLater);
            list[i]->setSslConfiguration(conf);
        }
    }

    QList<QWebSocketServer*> list;
    list << m_webCmdServer << m_webDataServer << m_webAppServer;
    for(int i = 0 ; i < list.size() ; i++)
    {
        connect(list[i],&QWebSocketServer::acceptError,this,[this](QAbstractSocket::SocketError socketError){
            QWebSocketServer *pServer = dynamic_cast<QWebSocketServer*>(sender());
            if(ServerManager::GetInstance()->GetSystemAPI()->iProjectClientAPI && pServer)
            {
                QString strMsg = pServer->serverName();
                strMsg += "----acceptError:";
                strMsg += QString::number(socketError);
                strMsg += "errorString:" + pServer->errorString();
                ServerManager::GetInstance()->GetSystemAPI()->iProjectClientAPI->iSaveLog(bc::LOG_QT, strMsg.toStdString());
            }
        });
        connect(list[i],&QWebSocketServer::serverError,this,[this](QWebSocketProtocol::CloseCode closeCode){
            QWebSocketServer *pServer = dynamic_cast<QWebSocketServer*>(sender());
            if(ServerManager::GetInstance()->GetSystemAPI()->iProjectClientAPI && pServer)
            {
                QString strMsg = pServer->serverName();
                strMsg += "----serverError:";
                strMsg += QString::number(closeCode);
                strMsg += "errorString:" + pServer->errorString();
                ServerManager::GetInstance()->GetSystemAPI()->iProjectClientAPI->iSaveLog(bc::LOG_QT, strMsg.toStdString());
            }
        });
//        connect(list[i],&QWebSocketServer::originAuthenticationRequired,this,[this](QWebSocketCorsAuthenticator *pAuthenticator){
//            QWebSocketServer *pServer = dynamic_cast<QWebSocketServer*>(sender());
//            pAuthenticator->setAllowed(true);
//            if(ServerManager::GetInstance()->GetSystemAPI()->iProjectClientAPI && pServer)
//            {
//                QString strMsg = pServer->serverName();
//                strMsg += "----originAuthenticationRequired:";
//                ServerManager::GetInstance()->GetSystemAPI()->iProjectClientAPI->iSaveLog(bc::LOG_QT, strMsg.toStdString());
//            }
//        });
    }

    m_pSocketHandler = new WebSocketHandler("appSocket",this);
    m_pSocketHandler->setName("appSocket");
    m_pSocketHandler->setCallback(true);
    connect(m_pSocketHandler,&WebSocketHandler::disconnected,this,&WebSocketTransfer::slotBusinessDisconnected);

    emit newAppSocketConnected(m_pSocketHandler);

    nCmdPort= ServerManager::GetInstance()->GetMainParam()->nWebPort+1;
    if(m_webCmdServer->listen(QHostAddress::Any, nCmdPort))
    {
        qDebug()<<"OK: USER COMMAND PORT is started on: "<< nCmdPort;
        connect(m_webCmdServer,&QWebSocketServer::newConnection,this,&WebSocketTransfer::setCmdSocketConnected);
    }
    else qDebug()<<"ERROR: Web-Server is not started on port:"<<nCmdPort;


    if(m_webAppServer->listen(QHostAddress::Any, nCmdPort+2))
    {
        qDebug()<<"OK: APP COMMAND PORT is started on: "<< nCmdPort+2;
        connect(m_webAppServer,&QWebSocketServer::newConnection,this,&WebSocketTransfer::setAppSocketConnected);
    }
    else qDebug()<<"ERROR: Web-Server is not started on port:"<<nCmdPort+2;

    if(m_webDataServer->listen(QHostAddress::Any, nCmdPort+3))
    {
        qDebug()<<"OK: DATA STREAM PORT is started on: "<< nCmdPort+3;
        connect(m_webDataServer,&QWebSocketServer::newConnection,this,&WebSocketTransfer::setDataSocketConnected);
    }
    else qDebug()<<"ERROR: Web-Server is not started on port:"<<nCmdPort+3;


//     ServerManager::GetInstance()->GetSystemAPI()->iProtocol->WriteLog(LOG_LEVEL_DEBUG, "GetMainParam()->nCommonMode=%d\n",GetMainParam()->nCommonMode);
}

void WebSocketTransfer::slotStartClient()
{
    qDebug()<<"slotStartClient";
    m_webAppServer= new QWebSocketServer("App Socket Server",QWebSocketServer::NonSecureMode,this);
    QList<QWebSocketServer*> list;
    list << m_webAppServer;
    for(int i = 0 ; i < list.size() ; i++)
    {
        connect(list[i],&QWebSocketServer::acceptError,this,[this](QAbstractSocket::SocketError socketError){
            QWebSocketServer *pServer = dynamic_cast<QWebSocketServer*>(sender());
            if(ServerManager::GetInstance()->GetSystemAPI()->iProjectClientAPI && pServer)
            {
                QString strMsg = pServer->serverName();
                strMsg += "----acceptError:";
                strMsg += QString::number(socketError);
                strMsg += "errorString:" + pServer->errorString();
                ServerManager::GetInstance()->GetSystemAPI()->iProjectClientAPI->iSaveLog(bc::LOG_QT, strMsg.toStdString());
            }
        });
        connect(list[i],&QWebSocketServer::serverError,this,[this](QWebSocketProtocol::CloseCode closeCode){
            QWebSocketServer *pServer = dynamic_cast<QWebSocketServer*>(sender());
            if(ServerManager::GetInstance()->GetSystemAPI()->iProjectClientAPI && pServer)
            {
                QString strMsg = pServer->serverName();
                strMsg += "----serverError:";
                strMsg += QString::number(closeCode);
                strMsg += "errorString:" + pServer->errorString();
                ServerManager::GetInstance()->GetSystemAPI()->iProjectClientAPI->iSaveLog(bc::LOG_QT, strMsg.toStdString());
            }
        });
    }

    m_pSocketHandler = new WebSocketHandler("appSocket",this);
    m_pSocketHandler->setName("appSocket");
    m_pSocketHandler->setCallback(true);
    connect(m_pSocketHandler,&WebSocketHandler::disconnected,this,&WebSocketTransfer::slotBusinessDisconnected);

    emit newAppSocketConnected(m_pSocketHandler);

    int nCmdPort= ServerManager::GetInstance()->GetMainParam()->nWebPort+1;
        qDebug()<<"AppSocket---" <<nCmdPort;
    if(m_webAppServer->listen(QHostAddress::Any, nCmdPort+2))
    {
        qDebug()<<"OK: APP COMMAND PORT is started on: "<< nCmdPort+2;
        connect(m_webAppServer,&QWebSocketServer::newConnection,this,&WebSocketTransfer::setAppSocketConnected);
    }
}

void WebSocketTransfer::stop()
{
    if(m_webCmdServer)
    {
        if(m_webCmdServer->isListening())
            m_webCmdServer->close();
    }


    if(m_webDataServer)
    {
        if(m_webDataServer->isListening())
            m_webDataServer->close();
    }

    if(m_webAppServer)
    {
        if(m_webAppServer->isListening())
            m_webAppServer->close();
    }

    emit finished();
}


 void WebSocketTransfer::setAutoLogin(QString strLogin)
 {
     QString user= ServerManager::GetInstance()->GetMainParam()->szUser;
     QString pass= ServerManager::GetInstance()->GetMainParam()->szPass;
     QString str="inputLogin="+user+"&inputPass="+pass;

     if(strLogin==str)
     {
         m_bAutoLogin=true;
     }
 }

void WebSocketTransfer::setAuthStatus(quint16 nAuthStatus)
{
    m_nAuthStatus=nAuthStatus;
}


void WebSocketTransfer::checkRemoteAuthentication(const QByteArray &uuid, const QByteArray &nonce, const QByteArray &request)
{
    foreach(WebSocketHandler *socketHandler, m_cmdSockets)
        socketHandler->checkRemoteAuthentication(uuid,nonce,request);
}

void WebSocketTransfer::setRemoteAuthenticationResponse(const QByteArray &uuidDst, const QByteArray &uuidSrc, const QByteArray &nameSrc)
{
    qDebug()<<"WebSocketTransfer::setRemoteAuthenticationResponse dst:"<<uuidDst.toBase64()<<"src:"<<uuidSrc.toBase64();

    foreach(WebSocketHandler *socketHandler, m_cmdSockets)
    {
        if(socketHandler->getUuid() == uuidDst)
        {
            socketHandler->setRemoteAuthenticationResponse(uuidSrc, nameSrc);
            break;
        }
    }
}

void WebSocketTransfer::slotReset()
{
      stopSending(true);
}


void WebSocketTransfer::clearSocket()
{
    if(m_cmdSockets.length()>0)
    {
        for(int i=0;i<m_cmdSockets.length();i++)
        {

            QWebSocket *socket = m_cmdSockets[i]->getSocket();
            socket->deleteLater();

            m_cmdSockets[i]->disconnect();
            m_cmdSockets[i]->deleteLater();
        }

        m_cmdSockets.clear();
    }
}


void WebSocketTransfer::setCmdSocketConnected()
{
    QWebSocket    *webSocket = m_webCmdServer->nextPendingConnection();
    QString       hostAddr = webSocket->peerAddress().toString();
	char          szClientIp[40] = { 0 };

    int last=hostAddr.lastIndexOf(":");
    if(last>=0)
    {
        hostAddr=hostAddr.right(hostAddr.length()-last-1);
    }

    strcpy(szClientIp,hostAddr.toLocal8Bit().toStdString().c_str());

//    clearSocket();
    if(m_cmdSockets.length() > 1)
    {
        //已经有添加了，直接返回，目前一个服务器 仅支持1对1
        return;
    }

    if (!strcmp(ServerManager::GetInstance()->GetMainParam()->szClient, szClientIp)) //如果是本机刷新
	{
        WebSocketHandler *socketHandler = new WebSocketHandler("cmdSocket",this);
        socketHandler->setName("cmdSocket");
        connect(this,&WebSocketTransfer::sigWebRtcFailed,socketHandler,&WebSocketHandler::slotWebrtcFailed);


		socketHandler->setLoginPass();

		socketHandler->setCmdSocket(webSocket, m_bAutoLogin);

        strcpy(ServerManager::GetInstance()->GetMainParam()->szClient, szClientIp);
        stopSending(false);

		m_cmdSockets.append(socketHandler);
        connect(socketHandler,&WebSocketHandler::sigCmdDisconnected,this,[this](WebSocketHandler *pHandler)
        {
//            for(int i = 0 ; i < m_cmdSockets.size() ; ++i)
//            {
//                m_cmdSockets[i]->deleteLater();
//            }
            m_cmdSockets.clear();
        });

		emit newCmdSocketConnected(socketHandler);


	}
	else if (m_cmdSockets.length() <1)
	{
        WebSocketHandler *socketHandler = new WebSocketHandler("cmdSocket",this);
        connect(this,&WebSocketTransfer::sigWebRtcFailed,socketHandler,&WebSocketHandler::slotWebrtcFailed);
        socketHandler->setName("cmdSocket");


		socketHandler->setLoginPass();

		socketHandler->setCmdSocket(webSocket, m_bAutoLogin);

        strcpy(ServerManager::GetInstance()->GetMainParam()->szClient, szClientIp);

		m_cmdSockets.append(socketHandler);
        connect(socketHandler,&WebSocketHandler::sigCmdDisconnected,this,[this](WebSocketHandler *pHandler)
        {
//            for(int i = 0 ; i < m_cmdSockets.size() ; ++i)
//            {
//                m_cmdSockets[i]->deleteLater();
//            }
            m_cmdSockets.clear();
        });

		emit newCmdSocketConnected(socketHandler);


	}
	

}

void WebSocketTransfer::setDataSocketConnected()
{
    QWebSocket *webSocket = m_webDataServer->nextPendingConnection();

    WebSocketHandler *socketHandler = new WebSocketHandler("dataSocket",this);
    connect(this,&WebSocketTransfer::sigWebRtcFailed,socketHandler,&WebSocketHandler::slotWebrtcFailed);

    QString strName="dataSocket";

    socketHandler->setName(strName);
    socketHandler->setDataSocket(webSocket);
    m_dataSockets.append(socketHandler);

    emit newDataSocketConnected(socketHandler);
}

void WebSocketTransfer::setAppSocketConnected()
{
    QWebSocket *webSocket = m_webAppServer->nextPendingConnection();
    connect(this,&WebSocketTransfer::sigWebRtcFailed,m_pSocketHandler,&WebSocketHandler::slotWebrtcFailed);
    m_pSocketHandler->setAppSocket(webSocket);
}

void WebSocketTransfer::clearCallback()
{
    if(ServerManager::GetInstance()->GetSystemAPI())
    {
        ServerManager::GetInstance()->SetCallback((fnCallBack)NULL,ENUM_CB_SEND_BY_WEBSOCKET);
        ServerManager::GetInstance()->SetIntPtrValue(NULL,INT_PTR_VALUE_WEB_SOCKET);
    }
}




void WebSocketTransfer::stopSending(bool bReset)
{



}

void WebSocketTransfer::slotBusinessDisconnected()
{
    emit businessDisconnected();
}

void WebSocketTransfer::static_Cloud_handle_func(const std::string& strCode,QList<QJsonValue> list)
{
    if(strCode == "webrtc_failed")
    {
        if(ServerManager::GetInstance()->GetSystemAPI()->iProjectClientAPI)
        {
            std::string strMsg = "receive cloud message--" + strCode;
            ServerManager::GetInstance()->GetSystemAPI()->iProjectClientAPI->iSaveLog(bc::LOG_QT,strMsg);
        }
        emit g_pThis->sigWebRtcFailed();
    }
}
