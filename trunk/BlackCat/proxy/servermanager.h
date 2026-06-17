#ifndef SERVERMANAGER_H
#define SERVERMANAGER_H

#include <QObject>
#include "BlackCat/basicdefine.h"
#include "BlackCat/glwidget.h"
#include "inputsimulator.h"
#include <QWebSocketServer>

class WebSocketTransfer;
class WebSocketHandler;
class ServerHttp;

typedef int(*fnCallBack)(INT_PTR wParam, INT_PTR lParam);
#define SIZE_UUID 16

typedef enum _CallBack_e
    {
        ENUM_CB_BACK,                     //返回

        ENUM_CB_TEST_SETPARAM,            //测试：设置参数

        ENUM_CB_USER_OBJECT,              //获取用户业务对象
        ENUM_CB_SEND_BY_WEBSOCKET,        //通过websocket发送
        ENUM_CB_RECV_BY_WEBSOCKET,        //通过websocket接收

        ENUM_CB_MAX
    }CallBack_E;

typedef enum _IntPtrValueType_e
    {
        INT_PTR_VALUE_APP_FACTORY = 1,    //应用工厂
        INT_PTR_VALUE_USER_OBJECT,
        INT_PTR_VALUE_PTZ_MONITOR,
        INT_PTR_VALUE_WEB_SOCKET,         //发送指令的websocket
        INT_PTR_VALUE_PLUGIN_MANAGER,

        INT_VALUE_CURSOR_X,                   //鼠标X
        INT_VALUE_CURSOR_Y,                   //鼠标Y

        INT_PTR_VALUE_MAX
    }IntPtrValueType_E;

class LocalWebSocketServerHandler : public QObject
{
    Q_OBJECT
public:
    LocalWebSocketServerHandler(int nPort);
    ~LocalWebSocketServerHandler();

    void SendToWebJsData(const QString &strJsData,const QString &strWebID);

signals:
    void sigSendToWeb(const QString &strJsData,const QString &strWebID);

public slots:
    void slotStartServer();
    void slotNewClient();

    void slotSendToWeb(const QString &strJsData,const QString &strWebID);

private:
    QWebSocketServer *m_pWebSocektServer;
    QMap<QString,QWebSocket*> m_mapWebSocketClient;
    int m_nPort;

};
/////////////////////////////////////////////////////////////////////

class ServerManager : public QObject
{
    Q_OBJECT

public:
    virtual ~ServerManager();
    static ServerManager* GetInstance();

    void                 SetSystemAPI(bc::ISystemAPI* pSystemAPI);
    bc::ISystemAPI*      GetSystemAPI();
    bc::ICloudServer*    GetMainParam();
    void                 SetTransfer(WebSocketTransfer* pTransfer);
    WebSocketTransfer*   GetTransfer();

    void                 SetCallback(fnCallBack pCallback,CallBack_E eType);
    fnCallBack           GetCallback(CallBack_E eType);
    void                 SetIntPtrValue(INT_PTR pValue,IntPtrValueType_E eType);
    INT_PTR              GetIntPtrValue(IntPtrValueType_E eType);
    void                 SetGLWidget(GLWidget *pWidget);
    QWidget*             GetGLWidget();

    void                 ExecCallback(CallBack_E eType, INT_PTR wParam, INT_PTR lParam);

    void StartServer();
    void startHttpServer(quint16 webPort, const QString &filesPath);
    void startWebSocketTransfer();
    void CreateSubscribeWebsocketServer();        //创建订阅websocket server
    void CreateSubScribeWebsocketClient(QString strHost,QString strUserToken,QString strSubId);
    void CloseSubScribeWebsocketClient();  //断开订阅websocket连接


    void StartLocalWebSocketServer();       //启动本地网页模式三维websocket server
    void SendToWebJsData(const QString &strJsData,const QString &strWebID);

protected:
    ServerManager();

    void sendUserToken();   //发送userToken

protected:
    static ServerManager*   s_pThis;

    bc::ISystemAPI*     m_pSystemAPI;
    WebSocketTransfer*  m_pWebSocketTransfer;
    ServerHttp *           m_serverHttp;
    InputSimulator*       m_inputSimulator;

    std::map<CallBack_E,fnCallBack> m_mapCallback;
    std::map<IntPtrValueType_E,INT_PTR> m_mapValue;
    QString                m_currentIp;
    GLWidget*           m_pGLWidget;
    QWebSocketServer*   m_pSubscribeWebsocketServer;    //订阅websocket
    QWebSocket*         m_pSubscribeWebsocket;
    QString             m_strUserToken;
    QString             m_strSubId;
    QWebSocket*         m_pSubscribeWebsocketClient;

    LocalWebSocketServerHandler *m_pLocalWebSocketServerHandler;

signals:
    void closeSignal();

public slots:
    void slotSetupCmdHandle(WebSocketHandler *webSocketHandler);
    void slotSetupDataHandle(WebSocketHandler *webSocketHandler);
    void slotSetupAppHandle(WebSocketHandler *webSocketHandler);

    void finishedWebSockeTransfer();
    void finishedWebSockeHandler();
    void updateCurrentIp();

    void slotNewSubscribWebsocket();
    void slotSubscribeTextMessageReceived(const QString &message);
    void slotSlotSubscribeClientConnected();

private:
    class GC // 垃圾回收类
    {
    public:
        GC()
        {
        }

        ~GC()
        {
            if (s_pThis != NULL)
            {
                s_pThis->deleteLater();
            }
        }
    };
    static GC gc;  //垃圾回收类的静态成员
};

#endif // SERVERMANAGER_H
