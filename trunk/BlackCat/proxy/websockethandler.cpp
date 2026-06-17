#include <QCryptographicHash>
#include <QDebug>
#include <QUuid>

//#include "HCClouderender.h"

#include "websockethandler.h"
#include "websockettransfer.h"
#ifdef WIN32
#include "windows.h"
#endif
#include "servermanager.h"
#ifdef __GNUC__
int GetTickCount()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}
#endif

#define TIME_HEARTBEAT  (30*1000)

static const QByteArray KEY_GET_IMAGE = "GIMG";
static const QByteArray KEY_BROWSER_IMAGE = "BIMG";
static const QByteArray KEY_IMAGE_PARAM = "IMGP";
static const QByteArray KEY_SET_SCALE_SIZE = "SCSZ";
static const QByteArray KEY_SET_KEY_STATE = "SKST";
static const QByteArray KEY_SET_CURSOR_POS = "SCUP";
static const QByteArray KEY_SET_CURSOR_DELTA = "SCUD";
static const QByteArray KEY_SET_MOUSE_KEY = "SMKS";
static const QByteArray KEY_SET_MOUSE_WHEEL = "SMWH";
static const QByteArray KEY_SET_NONCE = "STNC";
static const QByteArray KEY_SET_AUTH_REQUEST = "SARQ";
static const QByteArray KEY_SET_AUTH_RESPONSE = "SARP";
static const QByteArray KEY_CHECK_AUTH_REQUEST = "CARQ";
static const QByteArray KEY_CHECK_AUTH_RESPONSE = "CARP";
static const QByteArray KEY_SET_NAME = "STNM";
static const QByteArray KEY_APP_DATA_INTERFACE = "APDI";
static const QByteArray KEY_AUTO_LOGIN = "AULG";
static const QByteArray KEY_HEARTBEAT = "HEAT";

static const QByteArray KEY_DEBUG = "DBUG";

const int COMMAD_SIZE = 4;
const int REQUEST_MIN_SIZE = 6;

using namespace bc;


WebSocketHandler::WebSocketHandler(QString strName,QObject *parent) : QObject(parent),
    m_webSocket(Q_NULLPTR),
    m_timerReconnect(Q_NULLPTR),
    m_timerWaitResponse(Q_NULLPTR),
    m_timerHeartbeat(Q_NULLPTR),
    m_waitType(WaitTypeUnknown),
    m_nAuthenticated(AUTH_NONE)
{
    setName(strName);
    connect(this,SIGNAL(sigSendJsData(QString)),this,SLOT(sendAppText(QString)));
}

void WebSocketHandler::createSocket()
{
    if(m_webSocket)
        return;

    m_webSocket = new QWebSocket("",QWebSocketProtocol::VersionLatest,this);
    connect(m_webSocket, &QWebSocket::stateChanged, this, &WebSocketHandler::socketStateChanged);
    connect(m_webSocket, &QWebSocket::textMessageReceived,this, &WebSocketHandler::textMessageReceived);
    connect(m_webSocket, &QWebSocket::binaryMessageReceived,this, &WebSocketHandler::binaryMessageReceived);

    if(!m_timerReconnect)
    {
        m_timerReconnect = new QTimer(this);
        connect(m_timerReconnect, &QTimer::timeout, this, &WebSocketHandler::timerReconnectTick);
    }

    m_webSocket->open(QUrl(m_url));
}

void WebSocketHandler::removeSocket()
{
    if(m_timerReconnect)
    {
        if(m_timerReconnect->isActive())
            m_timerReconnect->stop();

        m_timerReconnect->deleteLater();
        m_timerReconnect = Q_NULLPTR;
    }

    if(m_timerWaitResponse)
    {
        if(m_timerWaitResponse->isActive())
            m_timerWaitResponse->stop();

        m_timerWaitResponse->deleteLater();
        m_timerWaitResponse = Q_NULLPTR;
    }

    if(m_webSocket)
    {
        if(m_webSocket->state() != QAbstractSocket::UnconnectedState)
            m_webSocket->close();

        m_webSocket->disconnect();
        m_webSocket->deleteLater();
        m_webSocket = Q_NULLPTR;
    }

    emit finished();
}

void WebSocketHandler::setUrl(const QString &url)
{
    m_url = url;
    qDebug()<<"SocketWeb::setUrl"<<url<<QUrl(url).isValid();
}


void WebSocketHandler::setName(const QString &name)
{
    m_name = name;
}


QString WebSocketHandler::getName()
{
    return m_name;
}

QByteArray WebSocketHandler::getUuid()
{
    return m_uuid;
}


void WebSocketHandler::setLoginPass()
{
    QString  pass= ServerManager::GetInstance()->GetMainParam()->szPass;

    m_login = ServerManager::GetInstance()->GetMainParam()->szUser;
    m_pass = QCryptographicHash::hash(pass.toUtf8(),QCryptographicHash::Md5).toHex();


}


void WebSocketHandler::setCmdSocket(QWebSocket *webSocket,bool bAutoLogin)
{
    if(!webSocket)
        return;
    if(m_webSocket)
    {
        return;
    }

    m_webSocket = webSocket;
    if(ServerManager::GetInstance()->GetSystemAPI()->iProjectClientAPI)
    {
        QString str = "CmdWebSocketServer connect one client:IP=";
        str += m_webSocket->peerAddress().toString();
        str += ",port=" + QString::number(m_webSocket->peerPort());
        ServerManager::GetInstance()->GetSystemAPI()->iProjectClientAPI->iSaveLog(bc::LOG_QT,str.toStdString());
    }
    connect(m_webSocket, &QWebSocket::textMessageReceived,this, &WebSocketHandler::textMessageReceived);
    connect(m_webSocket, &QWebSocket::binaryMessageReceived,this, &WebSocketHandler::binaryMessageReceived);
    connect(m_webSocket, &QWebSocket::disconnected,this, [this](){
        QWebSocket *pWebSocket = dynamic_cast<QWebSocket*>(sender());
        if(ServerManager::GetInstance()->GetSystemAPI()->iProjectClientAPI && pWebSocket)
        {
            QString str = "CmdWebSocketServer disconnected one client:IP=";
            str += pWebSocket->peerAddress().toString();
            str += ",port=" + QString::number(pWebSocket->peerPort());
            ServerManager::GetInstance()->GetSystemAPI()->iProjectClientAPI->iSaveLog(bc::LOG_QT,str.toStdString());
        }
        emit sigCmdDisconnected(this);
    });
    connect(m_webSocket, &QWebSocket::disconnected,this, &WebSocketHandler::socketDisconnected);


    m_uuid = QUuid::createUuid().toRfc4122();
    m_nonce = QUuid::createUuid().toRfc4122();


    QByteArray data;

    if(!bAutoLogin)
    {
        data.append(KEY_SET_NONCE);
        data.append(arrayFromUint16(static_cast<quint16>(m_nonce.size())));
        data.append(m_nonce);
    }
    else
    {
         QByteArray str=m_login.toUtf8()+QByteArray(":")+m_pass.toUtf8();

        int nSize1=m_nonce.size();
        int nSize2=str.size();

        data.append(KEY_AUTO_LOGIN);

        data.append(arrayFromUint16(static_cast<quint16>(nSize1+nSize2+2)));
        data.append(arrayFromUint16(static_cast<quint16>(nSize1)));
        data.append(m_nonce);
        data.append(str);
    }

//    if(ServerManager::GetInstance()->GetSystemAPI()->iProjectClientAPI)
//    {
//        QString str = "CmdWebSocketServer will sendBinaryMessage with login!";
//        ServerManager::GetInstance()->GetSystemAPI()->iProjectClientAPI->iSaveLog(bc::LOG_QT,str.toStdString());
//    }
    sendBinaryMessage(data);
}

void WebSocketHandler::setDataSocket(QWebSocket *webSocket)
{
    if(!webSocket)
        return;
    if(m_webSocket)
    {
        return;
    }

    m_webSocket = webSocket;
    if(ServerManager::GetInstance()->GetSystemAPI()->iProjectClientAPI)
    {
        QString str = "DataWebSocketServer connect one client:IP=";
        str += m_webSocket->peerAddress().toString();
        str += ",port=" + QString::number(m_webSocket->peerPort());
        ServerManager::GetInstance()->GetSystemAPI()->iProjectClientAPI->iSaveLog(bc::LOG_QT,str.toStdString());
    }
    connect(m_webSocket, &QWebSocket::disconnected,this, &WebSocketHandler::socketDisconnected);

    connect(m_webSocket, &QWebSocket::textMessageReceived,this, &WebSocketHandler::textMessageReceived);
    connect(m_webSocket, &QWebSocket::binaryMessageReceived,this, &WebSocketHandler::binaryMessageReceived);
    connect(m_webSocket, &QWebSocket::disconnected,this, [this](){
        QWebSocket *pWebSocket = dynamic_cast<QWebSocket*>(sender());
        if(ServerManager::GetInstance()->GetSystemAPI()->iProjectClientAPI && pWebSocket)
        {
            QString str = "DataWebSocketServer disconnected one client:IP=";
            str += pWebSocket->peerAddress().toString();
            str += ",port=" + QString::number(pWebSocket->peerPort());
            ServerManager::GetInstance()->GetSystemAPI()->iProjectClientAPI->iSaveLog(bc::LOG_QT,str.toStdString());
        }
    });


}



void WebSocketHandler::setAppSocket(QWebSocket *webSocket)
{
    if(!webSocket)
        return;
    if(m_webSocket)
    {
        return;
    }

    m_webSocket = webSocket;
    if(ServerManager::GetInstance()->GetSystemAPI()->iProjectClientAPI)
    {
        QString str = "AppWebSocketServer connect one client:IP=";
        str += m_webSocket->peerAddress().toString();
        str += ",port=" + QString::number(m_webSocket->peerPort());
        ServerManager::GetInstance()->GetSystemAPI()->iProjectClientAPI->iSaveLog(bc::LOG_QT,str.toStdString());
    }
    connect(m_webSocket, &QWebSocket::disconnected,this, &WebSocketHandler::socketDisconnected);

    connect(m_webSocket, &QWebSocket::textMessageReceived,this, &WebSocketHandler::textMessageReceived);
    connect(m_webSocket, &QWebSocket::binaryMessageReceived,this, &WebSocketHandler::binaryMessageReceived);
    connect(m_webSocket, &QWebSocket::disconnected,this, [this](){
        QWebSocket *pWebSocket = dynamic_cast<QWebSocket*>(sender());
        if(ServerManager::GetInstance()->GetSystemAPI()->iProjectClientAPI)
        {
            QString str = "AppWebSocketServer disconnected one client:IP=";
            str += pWebSocket->peerAddress().toString();
            str += ",port=" + QString::number(pWebSocket->peerPort());
            ServerManager::GetInstance()->GetSystemAPI()->iProjectClientAPI->iSaveLog(bc::LOG_QT,str.toStdString());
        }
    });


}

QWebSocket *WebSocketHandler::getSocket()
{
    return m_webSocket;
}

void WebSocketHandler::sendImageParameters(const QSize &imageSize, int rectWidth)
{
    if(!m_nAuthenticated)
        return;

    QByteArray data;
    data.append(KEY_IMAGE_PARAM);
    data.append(arrayFromUint16(6));
    data.append(arrayFromUint16(static_cast<quint16>(imageSize.width())));
    data.append(arrayFromUint16(static_cast<quint16>(imageSize.height())));
    data.append(arrayFromUint16(static_cast<quint16>(rectWidth)));

    sendBinaryMessage(data);
}

void WebSocketHandler::sendImageData(const QByteArray &imageData)
{

    sendBinaryMessage(imageData);
}


void WebSocketHandler::sendAppBinary(const QByteArray &appData)
{

    sendBinaryMessage(appData);
}


void WebSocketHandler::sendAppData(char *pData,int nLength)
{

    QByteArray data;

    data.append(pData,nLength);

    sendBinaryMessage(data);

}


void WebSocketHandler::setCallback(bool bSet)
{
    if(ServerManager::GetInstance()->GetSystemAPI())
    {
        if(bSet)
        {
            ServerManager::GetInstance()->SetCallback((fnCallBack)&WebSocketHandler::sendJsData,ENUM_CB_SEND_BY_WEBSOCKET);
            ServerManager::GetInstance()->SetIntPtrValue((INT_PTR)this,INT_PTR_VALUE_WEB_SOCKET);
        }
        else
        {
            ServerManager::GetInstance()->SetCallback((fnCallBack)NULL,ENUM_CB_SEND_BY_WEBSOCKET);
            ServerManager::GetInstance()->SetIntPtrValue((INT_PTR)NULL,INT_PTR_VALUE_WEB_SOCKET);
        }
    }
}

int WebSocketHandler::sendJsData(INT_PTR wParam,INT_PTR lParam)
{
    if(ServerManager::GetInstance()->GetSystemAPI())
    {
          WebSocketHandler* pThis=(WebSocketHandler*)ServerManager::GetInstance()->GetIntPtrValue(INT_PTR_VALUE_WEB_SOCKET);
          if(pThis)
          {
//              qDebug()<<"WebSocketHandler::sendJsData";
              QString str((char *)wParam);
              emit pThis->sigSendJsData(str);
//               pThis->sendAppText(str);
          }
          else
          {
//              qDebug()<<"WebSocketHandler:: INT_PTR_VALUE_WEB_SOCKET is NULL";
          }
    }

    return 0;
}


void WebSocketHandler::sendAppText(QString strData)
{
    if(m_webSocket)
    {
        if(m_webSocket->state() == QAbstractSocket::ConnectedState)
        {
            m_webSocket->sendTextMessage(strData);
//            qDebug()<<("WebSocket Send \n");
        }
        else
        {
            qDebug()<<("WebSocket unconnected \n");
        }
    }
    else
    {
        qDebug()<<"m_webSocket is NULL";
    }
}



void WebSocketHandler::sendName(const QString &name)
{
    if(!m_nAuthenticated)
        return;

    QByteArray nameArray = name.toUtf8();
    QByteArray data;
    data.append(KEY_SET_NAME);
    data.append(arrayFromUint16(static_cast<quint16>(nameArray.size())));
    data.append(nameArray);

    sendBinaryMessage(data);
}

void WebSocketHandler::checkRemoteAuthentication(const QByteArray &uuid, const QByteArray &nonce, const QByteArray &request)
{
    if(!m_nAuthenticated)
        return;


    if(uuid.size() != SIZE_UUID ||
       nonce.size() != SIZE_UUID ||
       request.size() != SIZE_UUID)
        return;

    QByteArray data;
    data.append(KEY_CHECK_AUTH_REQUEST);
    data.append(arrayFromUint16(static_cast<quint16>(SIZE_UUID * 3)));
    data.append(uuid);
    data.append(nonce);
    data.append(request);

    sendBinaryMessage(data);
}

void WebSocketHandler::setRemoteAuthenticationResponse(const QByteArray &uuid, const QByteArray &name)
{

    stopWaitResponseTimer();

    QByteArray data;
    data.append(KEY_CHECK_AUTH_RESPONSE);
    data.append(arrayFromUint16(static_cast<quint16>(SIZE_UUID + name.size())));
    data.append(uuid);
    data.append(name);

    sendBinaryMessage(data);
}



void WebSocketHandler::doLoginSuccess()
{
    m_nAuthenticated = AUTH_SUCCESS;

     m_nLastTime=::GetTickCount();
  /*  if (!m_timerHeartbeat)
    {
        m_timerHeartbeat = new QTimer(this);

        connect(m_timerHeartbeat, &QTimer::timeout, this, &WebSocketHandler::timerHeartbeat);
		
		 m_timerHeartbeat->start(TIME_HEARTBEAT);
    }*/

   // GetTransfer()->setAuthStatus(m_nAuthenticated);


}



QByteArray WebSocketHandler::getHashSum(const QByteArray &nonce, const QString &login, const QString &pass)
{
    QString sum = login + pass;

    QByteArray concatFirst = QCryptographicHash::hash(sum.toUtf8(),QCryptographicHash::Md5).toBase64();
    concatFirst.append(nonce.toBase64());
    QByteArray result = QCryptographicHash::hash(concatFirst,QCryptographicHash::Md5).toBase64();
    return result;
}

void WebSocketHandler::getLoginInfo(const QByteArray &data, QString &strUser,QString &strPass)
{
    int nPos=0;

    nPos=data.indexOf(",");

    strUser=data.left(nPos).data();
    strPass=data.mid(nPos+1).data();


}

void WebSocketHandler::newData(const QByteArray &command, const QByteArray &data)
{
//    qDebug()<<"DataParser::newData"<<command<<data;

    if(!m_nAuthenticated)
    {
        //HCCloudeRender::GetInstance()->SetStart(true);

        if(command == KEY_SET_AUTH_REQUEST)
        {
            if(GetTransfer()->getAuthStatus())
            {
                sendAuthenticationResponse(AUTH_TWO_TIMES);
                return;
            }


            m_nAuthenticated = AUTH_NONE;
            if(!ServerManager::GetInstance()->GetMainParam()->nVerify)
            {
                doLoginSuccess();
            }
            else if(ServerManager::GetInstance()->GetMainParam()->nVerify==1)
            {
                QString theUser;
                QString thePass;
                QByteArray theResult;

                getLoginInfo(data,theUser, thePass);
                qDebug()<<"user:"<<theUser<<"pass:"<<thePass;
                qDebug()<<"user2:"<<m_login<<"pass2:"<<m_pass;
                if(thePass==m_pass  && theUser==m_login)
                {
                   doLoginSuccess();
                }
            }
            else if(ServerManager::GetInstance()->GetMainParam()->nVerify==2)
            {
                /*
                QString theUser;
                QString thePass;

                getLoginInfo(data,theUser, thePass);
                if(m_pRemoteDesktopUnit->DoLogin(theUser,thePass)>0)
                {
                     doLoginSuccess();
                }*/

            }


            sendAuthenticationResponse(m_nAuthenticated);
            qDebug()<<"Authentication attempt: "<<m_nAuthenticated;




            return;
        }
        else if(command == KEY_SET_AUTH_RESPONSE)
        {
            bool authState = uint16FromArray(data);

            if(authState)
            {
                m_nAuthenticated = AUTH_SUCCESS;
                sendName(m_name);
            }
            else m_nAuthenticated = AUTH_NONE;

            emit authenticatedStatus(m_nAuthenticated);

            return;
        }
        else
        {
            qDebug()<<"DataParser::newData"<<command<<data;
            debugHexData(data);
        }

        return;
    }
    if (command == KEY_DEBUG)
    {
        QString strdata = data;

        qDebug() << "DataParser::debug:" << strdata;

    }
    else if(command == KEY_HEARTBEAT)
    {
        m_nLastTime=::GetTickCount();
    }
    else if(command == KEY_GET_IMAGE)
    {
        m_nLastTime=::GetTickCount();

        QHostAddress hostAddr=m_webSocket->peerAddress();
        QString host=hostAddr.toString();
        int     port= ServerManager::GetInstance()->GetMainParam()->nWebPort+2;

        int last=host.lastIndexOf(":");
        if(last>=0)
        {
            host=host.right(host.length()-last-1);
        }
        ServerManager::GetInstance()->GetMainParam()->nCommonMode = 2;
        qDebug()<<"sigSetClient"<<host<<":"<<port;
        emit sigSetClient(host,port);


        sendName(m_name);
        emit getDesktop();
    }
	else if (command == KEY_BROWSER_IMAGE)
	{

        ServerManager::GetInstance()-> GetMainParam()->nCommonMode = 3;
		sendName(m_name);
		emit getDesktop();
	}
    else if(command == KEY_SET_SCALE_SIZE)
    {
        if(data.size() >= 4)
        {
            quint16 width = uint16FromArray(data.mid(0,2));
            quint16 height = uint16FromArray(data.mid(2,2));
            emit setScaleSize(width, height);
        }
    }

    else if(command == KEY_SET_CURSOR_POS)
    {
        if(data.size() >= 4)
        {
            quint16 posX = uint16FromArray(data.mid(0,2));
            quint16 posY = uint16FromArray(data.mid(2,2));
            emit setMouseMove(posX, posY);
        }
    }
    else if(command == KEY_SET_CURSOR_DELTA)
    {
        if(data.size() >= 4)
        {
            qint16 deltaX = static_cast<qint16>(uint16FromArray(data.mid(0,2)));
            qint16 deltaY = static_cast<qint16>(uint16FromArray(data.mid(2,2)));
            emit setMouseDelta(deltaX, deltaY);
        }
    }
    else if(command == KEY_SET_KEY_STATE)
    {
        if(data.size() >= 4)
        {
            quint16 keyCode = uint16FromArray(data.mid(0,2));
            quint16 keyState = uint16FromArray(data.mid(2,2));
            emit setKeyPressed(keyCode,static_cast<bool>(keyState));
        }
    }
    else if(command == KEY_SET_MOUSE_KEY)
    {
        if(data.size() >= 4)
        {
            quint16 keyCode = uint16FromArray(data.mid(0,2));
            quint16 keyState = uint16FromArray(data.mid(2,2));
            emit setMousePressed(keyCode,static_cast<bool>(keyState));
        }
    }
    else if(command == KEY_SET_MOUSE_WHEEL)
    {
        if(data.size() >= 4)
        {
            quint16 keyState = uint16FromArray(data.mid(2,2));
            emit setWheelChanged(static_cast<bool>(keyState));
        }
    }
    else if(command == KEY_SET_NAME)
    {
        m_name = QString::fromUtf8(data);
        qDebug()<<"New desktop connected:"<<m_name;
    }
    else if(command == KEY_CHECK_AUTH_REQUEST)
    {
        if(data.size() == SIZE_UUID * 3)
        {
            QByteArray uuid = data.mid(0, SIZE_UUID);
            QByteArray nonce = data.mid(SIZE_UUID, SIZE_UUID);
            QByteArray requset = data.mid(SIZE_UUID * 2, SIZE_UUID);

            sendRemoteAuthenticationResponse(uuid, nonce, requset);
        }
    }
    else if(command == KEY_CHECK_AUTH_RESPONSE)
    {
        if(data.size() == SIZE_UUID + 2)
        {
            QByteArray uuid = data.mid(0, SIZE_UUID);
            quint16 authResponse = uint16FromArray(data.mid(SIZE_UUID, 2));
            QByteArray nameUtf8 = m_name.toUtf8();
            emit remoteAuthenticationResponse(uuid, m_uuid, nameUtf8, static_cast<bool>(authResponse));
        }
    }
    else if(command == KEY_APP_DATA_INTERFACE)
    {
        emit appDataInterface(data);    //need connect the signal to app data,to be connect
    }
    else
    {
        qDebug()<<"DataParser::newData"<<command<<data;
        debugHexData(data);
    }
}


void WebSocketHandler::sendAuthenticationResponse(quint16 state)
{
    QByteArray data;
    data.append(KEY_SET_AUTH_RESPONSE);
    data.append(arrayFromUint16(6));
    data.append(arrayFromUint16(static_cast<quint16>(state)));

    data.append(arrayFromUint16(static_cast<quint16>(ServerManager::GetInstance()->GetMainParam()->nOriginWidth)));
    data.append(arrayFromUint16(static_cast<quint16>(ServerManager::GetInstance()->GetMainParam()->nOriginHeight)));


    sendBinaryMessage(data);

}

void WebSocketHandler::sendRemoteAuthenticationResponse(const QByteArray &uuid, const QByteArray &nonce, const QByteArray &request)
{
    bool result = request.toBase64() == getHashSum(nonce, m_login, m_pass);

    qDebug()<<"WebSocketHandler::sendRemoteAuthenticationResponse"<<uuid.toBase64();

    if(!result)
        return;

    if(uuid.size() != SIZE_UUID || nonce.size() != SIZE_UUID || request.size() != SIZE_UUID)
        return;

    QByteArray data;
    data.append(KEY_CHECK_AUTH_RESPONSE);
    data.append(arrayFromUint16(static_cast<quint16>(SIZE_UUID + 2)));
    data.append(uuid);
    data.append(arrayFromUint16(static_cast<quint16>(result)));

    sendBinaryMessage(data);
}






void WebSocketHandler::socketStateChanged(QAbstractSocket::SocketState state)
{
    switch(state)
    {
        case QAbstractSocket::UnconnectedState:
        {
            if(m_timerReconnect)
                if(!m_timerReconnect->isActive())
                    m_timerReconnect->start(5000);
            break;
        }
        case QAbstractSocket::HostLookupState:
        {
            break;
        }
        case QAbstractSocket::ConnectingState:
        {
            if(m_timerReconnect)
                if(!m_timerReconnect->isActive())
                    m_timerReconnect->start(5000);
            break;
        }
        case QAbstractSocket::ConnectedState:
        {
            qDebug()<<"SocketWeb::socketStateChanged: Connected to server.";
            emit connectedStatus(true);
            break;
        }
        case QAbstractSocket::ClosingState:
        {
            qDebug()<<"SocketWeb::socketStateChanged: Disconnected from server.";
            emit connectedStatus(false);
            m_nAuthenticated = AUTH_NONE;

            if(m_timerReconnect)
                if(!m_timerReconnect->isActive())
                    m_timerReconnect->start(5000);

            break;
        }
        default:
        {
            break;
        }
    }
}

void WebSocketHandler::socketDisconnected()
{
    emit disconnectedUuid(m_uuid);
    emit disconnected(this);

    m_webSocket = nullptr;
}

void WebSocketHandler::sendBinaryMessage(const QByteArray &data)
{
    if(m_webSocket)
        if(m_webSocket->state() == QAbstractSocket::ConnectedState)
            m_webSocket->sendBinaryMessage(data);
}



void WebSocketHandler::textMessageReceived(const QString &message)
{
    if(ServerManager::GetInstance()->GetSystemAPI())
    {
        QByteArray ba = message.toUtf8();
        const char *pData = ba.data();
        ServerManager::GetInstance()->ExecCallback(ENUM_CB_RECV_BY_WEBSOCKET,(INT_PTR)pData,(INT_PTR)message.length());

    }


   // qDebug() << "WebSocketHandler::textMessageReceived:" << message;
}

void WebSocketHandler::binaryMessageReceived(const QByteArray &data)
{
    QByteArray activeBuf = m_dataTmp;
    activeBuf.append(data);
    m_dataTmp.clear();

    int size = activeBuf.size();

    if(size == COMMAD_SIZE)
    {
        newData(data, QByteArray());
        return;
    }

    if(size < REQUEST_MIN_SIZE)
        return;

    int dataStep = 0;

    for(int i=0;i<size;++i)
    {
        QByteArray command = activeBuf.mid(dataStep, COMMAD_SIZE);
        quint16 dataSize = uint16FromArray(activeBuf.mid(dataStep + COMMAD_SIZE, 2));

        if(size >= (dataStep + COMMAD_SIZE + dataSize))
        {
            QByteArray payload = activeBuf.mid(dataStep + COMMAD_SIZE + 2, dataSize);
            dataStep += COMMAD_SIZE + 2 + dataSize;
            newData(command, payload);

            i = dataStep;
        }
        else
        {
            debugHexData(activeBuf);

            m_dataTmp = activeBuf.mid(dataStep, size - dataStep);

            if(m_dataTmp.size() > 2000)
                m_dataTmp.clear();

            break;
        }
    }
}


void WebSocketHandler::timerHeartbeat()
{
    if(GetTransfer()->getAuthStatus()>0)
    {
        int nNowTick=::GetTickCount();

        if((nNowTick-m_nLastTime)>(2*TIME_HEARTBEAT))
        {
            qDebug()<<"WebSocketHandler::timerHeartbeat timeout";
            //ClearResource();
        }

    }
}

void WebSocketHandler::timerReconnectTick()
{
    if(!m_webSocket)
    {
        return;
    }
    if(m_webSocket->state() == QAbstractSocket::ConnectedState)
    {
        m_timerReconnect->stop();
        return;
    }

    if(m_webSocket->state() == QAbstractSocket::ConnectingState)
        m_webSocket->abort();

    m_webSocket->open(QUrl(m_url));
}

void WebSocketHandler::startWaitResponseTimer(int msec, int type)
{
    if(!m_timerWaitResponse)
    {
        m_timerWaitResponse = new QTimer(this);
        connect(m_timerWaitResponse, &QTimer::timeout, this, &WebSocketHandler::timerWaitResponseTick);
    }

    m_timerWaitResponse->start(msec);
    m_waitType = type;
}

void WebSocketHandler::stopWaitResponseTimer()
{
    if(m_timerWaitResponse)
        if(m_timerWaitResponse->isActive())
            m_timerWaitResponse->stop();

    m_waitType = WaitTypeUnknown;
}

void WebSocketHandler::timerWaitResponseTick()
{
    m_timerWaitResponse->stop();

    if(m_waitType == WaitTypeRemoteAuth)
        sendAuthenticationResponse(false);

    m_waitType = WaitTypeUnknown;
}

void WebSocketHandler::debugHexData(const QByteArray &data)
{
    QString textHex;
    int dataSize = data.size();
    for(int i=0;i<dataSize;++i)
    {
        quint8 oneByte = static_cast<quint8>(data.at(i));
        textHex.append(QString::number(oneByte,16));

        if(i < dataSize-1)
            textHex.append("|");
    }

    qDebug()<<"DataParser::debugHexData:"<<textHex<<data;
}

QByteArray WebSocketHandler::arrayFromUint16(quint16 number)
{
    QByteArray buf;
    buf.resize(2);
    buf[0] = static_cast<char>(number);
    buf[1] = static_cast<char>(number >> 8);
    return buf;
}

quint16 WebSocketHandler::uint16FromArray(const QByteArray &buf)
{
    if(buf.count() == 2)
    {
        quint16 m_number;
        m_number = static_cast<quint16>(static_cast<quint8>(buf[0]) |
                static_cast<quint8>(buf[1]) << 8);
        return m_number;
    }
    else return 0x0000;
}

void WebSocketHandler::slotWebrtcFailed()
{
    if(m_webSocket)
    {
        emit m_webSocket->disconnected();
    }
}


