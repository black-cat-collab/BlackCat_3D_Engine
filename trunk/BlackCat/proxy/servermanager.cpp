#include "websockettransfer.h"
#include "servermanager.h"
#include <QApplication>
#include <QNetworkInterface>
#include "serverhttp.h"
#include <QList>
#include <QScreen>
#include <QSslKey>
#include <QDebug>
#include <QWebSocketServer>
#include <QWebSocket>
#include <QUuid>

using namespace bc;
ServerManager* ServerManager::s_pThis = nullptr;
ServerManager::GC ServerManager::gc;

LocalWebSocketServerHandler::LocalWebSocketServerHandler(int nPort):QObject()
{
    m_nPort = nPort;
    m_pWebSocektServer = nullptr;
}

LocalWebSocketServerHandler::~LocalWebSocketServerHandler()
{
    if(m_pWebSocektServer)
    {
        m_mapWebSocketClient.clear();
        m_pWebSocektServer->close();
        m_pWebSocektServer->deleteLater();
    }
}

void LocalWebSocketServerHandler::slotStartServer()
{
    m_pWebSocektServer = new QWebSocketServer(QStringLiteral("webServer"),QWebSocketServer::NonSecureMode,this);
    connect(m_pWebSocektServer, &QWebSocketServer::newConnection, this, &LocalWebSocketServerHandler::slotNewClient);
    m_pWebSocektServer->listen(QHostAddress::Any,m_nPort + 3);
    QString strMsg = QString("local Websocket server listen port=%1").arg(m_nPort);
    if(ServerManager::GetInstance()->GetSystemAPI()->iProjectClientAPI)
    {
        ServerManager::GetInstance()->GetSystemAPI()->iProjectClientAPI->iSaveLog(bc::LOG_QT,strMsg.toStdString());
    }
    qDebug()<<strMsg;

    connect(this,&LocalWebSocketServerHandler::sigSendToWeb,this,&LocalWebSocketServerHandler::slotSendToWeb);
}

void LocalWebSocketServerHandler::slotNewClient()
{
    if(m_pWebSocektServer->hasPendingConnections()) //有未处理的连接
    {
       QWebSocket *pWebSocketClient = m_pWebSocektServer->nextPendingConnection();
       connect(pWebSocketClient,&QWebSocket::textMessageReceived,this,[=](const QString &message){
               QByteArray ba = message.toUtf8();
               const char *pData = ba.data();
               ServerManager::GetInstance()->ExecCallback(ENUM_CB_RECV_BY_WEBSOCKET,(INT_PTR)pData,(INT_PTR)message.length());
           });

       connect(pWebSocketClient,&QWebSocket::disconnected,this,[=](){
               QWebSocket *pWebSocket = dynamic_cast<QWebSocket*>(sender());
               for(QMap<QString,QWebSocket*>::Iterator it = m_mapWebSocketClient.begin() ; it != m_mapWebSocketClient.end() ; ++it)
               {
                   if(it.value() == pWebSocket)
                   {
                       QString strMsg = QString("disconnected client=%1,port=%2").arg(pWebSocket->localAddress().toIPv4Address()).arg(pWebSocket->peerPort());
                       if(ServerManager::GetInstance()->GetSystemAPI()->iProjectClientAPI)
                       {
                           ServerManager::GetInstance()->GetSystemAPI()->iProjectClientAPI->iSaveLog(bc::LOG_QT,strMsg.toStdString());
                       }
                       m_mapWebSocketClient.erase(it);
                       break;
                   }
               }
           });

       QUuid uuid = QUuid::createUuid();
       QString strId = uuid.toString();
       strId .remove("{").remove("}").remove("-");
       m_mapWebSocketClient.insert(strId,pWebSocketClient);

       QString strMsg = QString("new websocket client=%1,port=%2").arg(pWebSocketClient->localAddress().toIPv4Address()).arg(pWebSocketClient->peerPort());
       if(ServerManager::GetInstance()->GetSystemAPI()->iProjectClientAPI)
       {
           ServerManager::GetInstance()->GetSystemAPI()->iProjectClientAPI->iSaveLog(bc::LOG_QT,strMsg.toStdString());
       }
       qDebug()<<strMsg;

       //告知网页uuid
       QJsonObject jUUid;
       jUUid.insert("client_uuid",strId);
       QString strRet;
       QJsonDocument doc(jUUid);
       strRet = QString(doc.toJson(QJsonDocument::Indented));
       SendToWebJsData(strRet,strId);
       if(ServerManager::GetInstance()->GetSystemAPI()->iProjectClientAPI)
       {
           QString strMsg = QString("client_uuid=%1").arg(strId);
           ServerManager::GetInstance()->GetSystemAPI()->iProjectClientAPI->iSaveLog(bc::LOG_QT,strMsg.toStdString());
       }
    }
}


void LocalWebSocketServerHandler::SendToWebJsData(const QString &strJsData,const QString &strWebID)
{
    emit sigSendToWeb(strJsData,strWebID);
}

void LocalWebSocketServerHandler::slotSendToWeb(const QString &strJsData,const QString &strWebID)
{
    QList<QWebSocket*> listClient;
    QString strClient;
    QStringList list = strWebID.split(":");
    if(list.size() >= 2)
    {
       strClient = list[1];
    }
    for(QMap<QString,QWebSocket*>::Iterator it = m_mapWebSocketClient.begin() ; it != m_mapWebSocketClient.end() ; ++it)
    {
       if(it.key() == strClient || strClient.isEmpty())
       {
           listClient.push_back(it.value());
       }
    }
    for(QWebSocket *pClient : listClient)
    {
       if(pClient && pClient->state() == QAbstractSocket::ConnectedState)
       {
           pClient->sendTextMessage(strJsData);
       }
    }
}

/////////////////////////////////////////////////////////////////////
ServerManager::ServerManager():
    m_pSystemAPI(nullptr),
    m_pWebSocketTransfer(nullptr),
    m_pGLWidget(nullptr),
    m_inputSimulator(nullptr),
    m_pSubscribeWebsocket(nullptr),
    m_pSubscribeWebsocketServer(nullptr),
    m_pSubscribeWebsocketClient(nullptr),
    m_pLocalWebSocketServerHandler(nullptr)
{
    s_pThis = this;
    m_serverHttp = new ServerHttp(this);
    m_inputSimulator = new InputSimulator(this),

    updateCurrentIp();
}

void ServerManager::SetSystemAPI(ISystemAPI* pSystemAPI)
{
    m_pSystemAPI = pSystemAPI;
}

ISystemAPI* ServerManager::GetSystemAPI()
{
    return m_pSystemAPI;
}

ServerManager::~ServerManager()
{
    if(m_inputSimulator)
    {
       delete m_inputSimulator;
    }

    if(m_pSubscribeWebsocketServer)
    {
        if(m_pSubscribeWebsocketServer->isListening())
        {
            m_pSubscribeWebsocketServer->close();
        }
    }
    if(m_pSubscribeWebsocketClient)
    {
        m_pSubscribeWebsocketClient->close();
    }
}

ServerManager* ServerManager::GetInstance()
{
    if(s_pThis == nullptr)
    {
        s_pThis = new ServerManager();
    }
    return s_pThis;
}

ICloudServer* ServerManager::GetMainParam()
{
    return m_pSystemAPI->iProtocolAPI->iGetCloudRenderManager()->iGetCloudParam();
}

void ServerManager::SetTransfer(WebSocketTransfer* pTransfer)
{
    m_pWebSocketTransfer = pTransfer;
}

WebSocketTransfer* ServerManager::GetTransfer()
{
    return m_pWebSocketTransfer;
}

void ServerManager::SetCallback(fnCallBack pCallback,CallBack_E eType)
{
    m_mapCallback[eType] = pCallback;
}

fnCallBack ServerManager::GetCallback(CallBack_E eType)
{
    return m_mapCallback[eType];
}

void ServerManager::SetIntPtrValue(INT_PTR pValue,IntPtrValueType_E eType)
{
    m_mapValue[eType] = pValue;
}

INT_PTR ServerManager::GetIntPtrValue(IntPtrValueType_E eType)
{
    return m_mapValue[eType];
}

void ServerManager::ExecCallback(CallBack_E eType, INT_PTR wParam, INT_PTR lParam)
{
    if(m_mapCallback.find(eType) != m_mapCallback.end())
    {
        m_mapCallback[eType](wParam,lParam);
    }
}


void ServerManager::startHttpServer(quint16 webPort, const QString &filesPath)
{
    m_serverHttp->setPort(static_cast<quint16>(webPort));
    m_serverHttp->setPath(filesPath);
    m_serverHttp->start();
}

void ServerManager::CreateSubscribeWebsocketServer()
{
    //启动订阅webscoekt server
    bool bHttps = (ServerManager::GetInstance()->GetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.strServerIp.find("https") != std::string::npos);
    m_pSubscribeWebsocketServer = new QWebSocketServer("Web Socket Server",bHttps?QWebSocketServer::SecureMode:QWebSocketServer::NonSecureMode,this);
    if(bHttps)
    {
        QList<QWebSocketServer*> list;
        list << m_pSubscribeWebsocketServer;
        for(int i = 0 ; i < list.size() ; i++)
        {
            QString strCrt = QApplication::applicationDirPath() + "/server.crt";
            QString strKey = QApplication::applicationDirPath() + "/server.key";
            QFile certFile(strCrt);
            QFile keyFile(strKey);
            if(!certFile.open(QIODevice::ReadOnly))
            {
                if(m_pSystemAPI->iProjectClientAPI)
                {
                    m_pSystemAPI->iProjectClientAPI->iSaveLog(LOG_QT, "certfile open failed...");
                }
            }
            if(!keyFile.open(QIODevice::ReadOnly))
            {
                if(m_pSystemAPI->iProjectClientAPI)
                {
                    m_pSystemAPI->iProjectClientAPI->iSaveLog(LOG_QT,"keyfile open failed...");
                }
            }
            QSslCertificate cert(&certFile, QSsl::Pem);
            QSslKey key(&keyFile, QSsl::Rsa, QSsl::Pem);
            certFile.close();
            keyFile.close();

            QSslConfiguration conf = list[i]->sslConfiguration();
            conf.setPeerVerifyMode(QSslSocket::VerifyNone);
            conf.setLocalCertificate(cert);
            conf.setPrivateKey(key);
            conf.setProtocol(QSsl::TlsV1_2OrLater);
            list[i]->setSslConfiguration(conf);
        }
    }

    int nCmdPort= ServerManager::GetInstance()->GetMainParam()->nWebPort+1;
    if(m_pSubscribeWebsocketServer->listen(QHostAddress::Any, nCmdPort + 5))
    {
        if(m_pSystemAPI->iProjectClientAPI)
        {
            std::string strMsg = "OK: subscribe websocket server is started on port:";
            strMsg += std::to_string(nCmdPort + 5);
            m_pSystemAPI->iProjectClientAPI->iSaveLog(LOG_QT,strMsg);
        }
        connect(m_pSubscribeWebsocketServer,&QWebSocketServer::newConnection,this,&ServerManager::slotNewSubscribWebsocket);
    }
    else
    {
        std::string strMsg = "ERROR: subscribe websocket server is not started on port:";
        strMsg += std::to_string(nCmdPort + 5);
        if (m_pSystemAPI->iProjectClientAPI)
        {
            m_pSystemAPI->iProjectClientAPI->iSaveLog(LOG_QT,strMsg);
        }
        
    }
}

void ServerManager::CreateSubScribeWebsocketClient(QString strHost,QString strUserToken,QString strSubId)
{
    m_strUserToken = strUserToken;
    m_strSubId = strSubId;

    QString strTemp = QString::fromLocal8Bit(m_pSystemAPI->iEngineAPI->iGetProjectConfig().sBootConfig.strServerIp.c_str());
    QString strUrl = "";
    bool bHttps = false;
    if(strTemp.startsWith("https"))
    {
        strUrl = QString("wss://%1").arg(strHost);

        bHttps = true;
    }
    else
    {
        strUrl = QString("ws://%1").arg(strHost);
    }
    if(m_pSubscribeWebsocketClient)
    {
        m_pSubscribeWebsocketClient->close();
        delete m_pSubscribeWebsocketClient;
        m_pSubscribeWebsocketClient = nullptr;
    }
    if(!m_pSubscribeWebsocketClient)
    {
        m_pSubscribeWebsocketClient = new QWebSocket("Web Socket Server",QWebSocketProtocol::VersionLatest,this);
        if(bHttps)
        {
            QSslConfiguration config = m_pSubscribeWebsocketClient->sslConfiguration();
            config.setPeerVerifyMode(QSslSocket::VerifyNone);
            config.setProtocol(QSsl::TlsV1_2OrLater);
            m_pSubscribeWebsocketClient->setSslConfiguration(config);
        }

        connect(m_pSubscribeWebsocketClient, &QWebSocket::textMessageReceived,this, &ServerManager::slotSubscribeTextMessageReceived);
        connect(m_pSubscribeWebsocketClient,&QWebSocket::connected,this, &ServerManager::slotSlotSubscribeClientConnected);
        m_pSubscribeWebsocketClient->open(QUrl(strUrl));
    }
//    else
//    {
//        if(m_pSubscribeWebsocketClient->state() == QAbstractSocket::SocketState::ClosingState)
//        {
//            m_pSubscribeWebsocketClient->open(strUrl);
//        }
//        else if(m_pSubscribeWebsocketClient->state() == QAbstractSocket::SocketState::ConnectedState)
//        {
//            sendUserToken();
//        }
//    }

    std::string strMsg = "do_subscribe_client:";
    strMsg += "host=" + strUrl.toLocal8Bit().toStdString();
    strMsg += ",userToken=" + m_strUserToken.toLocal8Bit().toStdString();
    strMsg += ",subId=" + m_strSubId.toLocal8Bit().toStdString();
    m_pSystemAPI->iProjectClientAPI->iSaveLog(LOG_QT,strMsg);
}

void ServerManager::CloseSubScribeWebsocketClient()
{
    if(m_pSubscribeWebsocketClient)
    {
        m_pSubscribeWebsocketClient->close();
        delete m_pSubscribeWebsocketClient;
        m_pSubscribeWebsocketClient = nullptr;

        std::string strMsg = "CloseSubScribeWebsocketClient";
        m_pSystemAPI->iProjectClientAPI->iSaveLog(LOG_QT,strMsg);
    }
}

void ServerManager::StartServer()
{
    m_inputSimulator->setInputWidget(m_pGLWidget);

    QList<QScreen*> screens = QApplication::screens();
    QScreen *screen = screens.at(0);
    ServerManager::GetInstance()->GetMainParam()->nOriginWidth=screen->geometry().width();
    GetMainParam()->nOriginHeight=screen->geometry().height();

    startHttpServer(static_cast<quint16>(GetMainParam()->nWebPort), "");
    startWebSocketTransfer();
}

void ServerManager::StartLocalWebSocketServer()
{
    qDebug()<<"StartLocalWebSocketServer";
    QThread *thread = new QThread(this);

    if(m_pSystemAPI->iProtocolAPI->iGetCloudRenderManager()->iGetRenderHost() != bc::STATUS_SUCCESS)
    {
        ServerManager::GetInstance()->GetMainParam()->nWebPort= ServerManager::GetInstance()->GetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.nQtCmdPort;
    }
    m_pLocalWebSocketServerHandler = new LocalWebSocketServerHandler(ServerManager::GetInstance()->GetMainParam()->nWebPort);
    connect(thread, &QThread::started, m_pLocalWebSocketServerHandler, &LocalWebSocketServerHandler::slotStartServer);
    connect(thread, &QThread::destroyed, this, [=](){
        m_pLocalWebSocketServerHandler->deleteLater();
    });
    m_pLocalWebSocketServerHandler->moveToThread(thread);
    thread->start();
}

void ServerManager::SendToWebJsData(const QString &strJsData,const QString &strWebID)
{
    if(m_pLocalWebSocketServerHandler)
    {
        m_pLocalWebSocketServerHandler->SendToWebJsData(strJsData,strWebID);
    }
}

void ServerManager::startWebSocketTransfer()
{
    QThread *thread = new QThread;


    m_pWebSocketTransfer = new WebSocketTransfer;


    connect(thread, &QThread::started, m_pWebSocketTransfer, &WebSocketTransfer::start);
//    connect(this, &ServerManager::closeSignal, m_pWebSocketTransfer, &WebSocketTransfer::stop);
//    connect(m_pWebSocketTransfer, &WebSocketTransfer::finished, this, &ServerManager::finishedWebSockeTransfer);
//    connect(m_pWebSocketTransfer, &WebSocketTransfer::finished, thread, &QThread::quit);
//    connect(thread, &QThread::finished, m_pWebSocketTransfer, &WebSocketTransfer::deleteLater);
//    connect(thread, &QThread::finished, thread, &QThread::deleteLater);

    connect(m_pWebSocketTransfer, &WebSocketTransfer::newCmdSocketConnected, this, &ServerManager::slotSetupCmdHandle,Qt::QueuedConnection);
    connect(m_pWebSocketTransfer, &WebSocketTransfer::newDataSocketConnected, this, &ServerManager::slotSetupDataHandle,Qt::QueuedConnection);
    connect(m_pWebSocketTransfer, &WebSocketTransfer::newAppSocketConnected, this, &ServerManager::slotSetupAppHandle,Qt::QueuedConnection);

    connect(m_serverHttp, &ServerHttp::sigReset, m_pWebSocketTransfer, &WebSocketTransfer::slotReset);
    connect(m_pWebSocketTransfer,&WebSocketTransfer::businessDisconnected,this,[this](){
        CloseSubScribeWebsocketClient();
    },Qt::QueuedConnection);


    m_pWebSocketTransfer->moveToThread(thread);
    thread->start();

}

void ServerManager::slotSetupCmdHandle(WebSocketHandler *webSocketHandler)
{
    if(!webSocketHandler || !m_inputSimulator)
        return;

    connect(webSocketHandler, &WebSocketHandler::setKeyPressed, m_inputSimulator, &InputSimulator::simulateKeyboard);
    connect(webSocketHandler, &WebSocketHandler::setMousePressed, m_inputSimulator, &InputSimulator::simulateMouseKeys);
    connect(webSocketHandler, &WebSocketHandler::setMouseMove, m_inputSimulator, &InputSimulator::simulateMouseMove);
    connect(webSocketHandler, &WebSocketHandler::setWheelChanged, m_inputSimulator, &InputSimulator::simulateWheelEvent);
    connect(webSocketHandler, &WebSocketHandler::setMouseDelta, m_inputSimulator, &InputSimulator::setMouseDelta);
}

void ServerManager::slotSetupDataHandle(WebSocketHandler *webSocketHandler)
{
    if(!webSocketHandler)
        return;
}

void ServerManager::slotSetupAppHandle(WebSocketHandler *webSocketHandler)
{
    if(!webSocketHandler)
        return;

    //不管本地还是云渲染都使用websocket进行通信
    //if(GetMainParam()->nCommonMode==3)
    {
         webSocketHandler->setCallback(true);
    }
}


void ServerManager::finishedWebSockeTransfer()
{
    m_pWebSocketTransfer = Q_NULLPTR;

    QApplication::quit();
}

void ServerManager::finishedWebSockeHandler()
{

    if(!m_pWebSocketTransfer)
        QApplication::quit();
}

void ServerManager::updateCurrentIp()
{
    QList<QHostAddress> my_addr_list;
    QList<QHostAddress> addresses = QNetworkInterface::allAddresses();

    for(int i=0;i<addresses.count();++i)
    {
        if(addresses.at(i).protocol() == QAbstractSocket::IPv4Protocol &&
                !addresses.at(i).toString().contains("127") &&
                !addresses.at(i).toString().contains("172"))
        {
            my_addr_list.append(addresses.at(i));
        }
    }

    if(my_addr_list.count() > 0)
    {
        for(int i=0;i<my_addr_list.count();i++)
        {
            qDebug()<<i << ":" <<my_addr_list.at(i).toString();
        }

        m_currentIp = my_addr_list.at(0).toString();
    }

}

void ServerManager::SetGLWidget(GLWidget *pWidget)
{
    m_pGLWidget = pWidget;
}

QWidget* ServerManager::GetGLWidget()
{
    return m_pGLWidget;
}

void ServerManager::slotNewSubscribWebsocket()
{
    m_pSubscribeWebsocket = m_pSubscribeWebsocketServer->nextPendingConnection();
    connect(m_pSubscribeWebsocket, &QWebSocket::textMessageReceived,this, &ServerManager::slotSubscribeTextMessageReceived);
    {
        QString strMsg = "connect_sub_server:host=" + m_pSubscribeWebsocket->localAddress().toString();
        m_pSystemAPI->iProjectClientAPI->iSaveLog(LOG_QT,strMsg.toLocal8Bit().toStdString());
    }
}

void ServerManager::slotSubscribeTextMessageReceived(const QString &message)
{
//    QWebSocket *pWebSocket = static_cast<QWebSocket*>(sender());
//    if(pWebSocket != m_pSubscribeWebsocket)
//    {
//        return;
//    }
    if(ServerManager::GetInstance()->GetSystemAPI())
    {
        std::string strJson = message.toStdString();
        bc::BCEvent _tEvent(-100001, (bc::EVENT_PARAM)strJson.data(), 0, 0);
        ServerManager::GetInstance()->GetSystemAPI()->iProjectClientAPI->iExecuteEvent(_tEvent);
    }
}

void ServerManager::slotSlotSubscribeClientConnected()
{
    sendUserToken();
}

void ServerManager::sendUserToken()
{
    if(m_pSubscribeWebsocketClient->state() == QAbstractSocket::SocketState::ConnectedState)
    {
        QJsonObject json_obj;//创建json对象
        json_obj.insert("userToken",m_strUserToken);
        json_obj.insert("subId",m_strSubId);
        QJsonDocument doc(json_obj);
        QString strRet = QString(doc.toJson(QJsonDocument::Indented));
        m_pSubscribeWebsocketClient->sendTextMessage(strRet);

        if(m_pSystemAPI->iProjectClientAPI)
        {
            QString str = "subscribe websocket connected,url=";
            str += m_pSubscribeWebsocketClient->requestUrl().toString();
            m_pSystemAPI->iProjectClientAPI->iSaveLog(LOG_QT,str.toLocal8Bit().toStdString().c_str());
        }
    }
}


