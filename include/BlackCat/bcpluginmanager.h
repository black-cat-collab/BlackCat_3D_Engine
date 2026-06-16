#ifndef BCPLUGINMANAGER_H
#define BCPLUGINMANAGER_H

#include <QObject>
#include <QThread>
#include <QDockWidget>
#include <QMultiMap>
#include <QStackedLayout>
#include "ipluginmanager.h"
#include "imainframeplugin.h"
#include "blackcat_global.h"
#include "bcwebview.h"
#include <QSystemTrayIcon>

class IMainFramePlugin;
class IWidgetPlugin;
class ILoginPlugin;
class MainWindow;
class MainFrame;
class BCWebView;

typedef bc::ISystemAPI* (*func_InitializeEngine)(const std::string strConfigFile);
typedef int (*func_UninitializeEngine)();

struct WebCoverPos {
    QPoint leftTop;
    QPoint rightBottom;
};

typedef struct _WebFunParamList_t
{
    QString strFunName;
    QJsonValue jsonData;
    std::vector<QVariant> vecParamList;
    QString strID;

    _WebFunParamList_t()
    {
        strFunName="";
        vecParamList.clear();
        strID = "";
    }
}WebFunParamList_T;

class BLACKCATSHARED_EXPORT BCPluginManager : public QObject, public IPluginManager, public bc::IEventProcessor
{
    Q_OBJECT
public:
    virtual ~BCPluginManager();

    virtual bool iLogin();

    virtual int  iLoadPlugin();
    virtual bool iIsPluginLoad() { return m_bPluginLoad; }

    virtual IWidgetPlugin* iGetPlugin(PluginType type);
    virtual QList<IWidgetPlugin*> iGetPlugins(PluginType type);

    virtual void iSetPluginPrefix(QString prefix);
    virtual void iSetConfig(QString config);

    virtual void iMouseMove();
    virtual void iMouseRelease(int x, int y);
    virtual void iMousePress(int x, int y);

    virtual void iSendCommand(IWidgetPlugin *pSender, ICommand *pCommand);

    virtual void iSetWindowSize(QSize size);
    virtual void iSetWindowSize(float fWidth, float fHeight);
    virtual float iGetWindowWidth();
    virtual float iGetWindowHeight();

    virtual void iStartGame();
    virtual bc::EventReturnType_e iProcessEvent(const bc::BCEvent& tEvent);
    virtual int				iGetProcessEventSort();
    virtual bc::ISystemAPI* iGetSystemAPI() { return m_pSystemAPI; }

    virtual void iSetDataChange(bool bChange, QString strID = "");
    virtual bool iIsDataChange(QString strID = "");

    //Web部分
    virtual void iAddWebClientBrowser(QWidget *pParent, QString strUrl);
    virtual void iEmitSignalRunJavaScript(QString strJson);
    virtual void iWebRunJavaScript(QString strJson);
    virtual void iSendToWebSocket(QString strJson);

    virtual bool iGetCanKeyEvent() {return m_bCanKeyDown;}
    virtual bool iCheckWebCanHandleScene(QPoint &point);
    virtual AppSetting* iGetAppSetting() { return m_pAppSetting; }
    static void  getFromWebSocket(INT_PTR wParam,INT_PTR lParam);

protected:
    BCPluginManager();

    virtual bool    initEngine();
    virtual void    iPostInitEngine() {}
    virtual bool    loadProjectDll();
    virtual void    releaseProjectDll();
    virtual void    loadDockPlugin();
    virtual int     loadMenuPlugin();
    virtual int     loadToolPlugin();
    virtual int     loadMainFramePlugin();
    virtual bool    loadLoginPlugin();
    virtual void    loadScenePlugin();
    virtual void    loadSceneControlPlugin();
    virtual void    initSize();
    virtual int     praseWebFunParam(WebFunParamList_T &stParam, const QByteArray &strjson);
    virtual void    matchTaskFunClass(WebFunParamList_T &param);
    virtual bool    handleDefaultWebCommand(WebFunParamList_T &param);

    static void static_forvs_connect_fn(const std::string& strCommond,std::map<std::string,INT_PTR> mapExtraParam, void* pInData, void* pOutData);

protected:
    bc::ISystemAPI*         m_pSystemAPI;
    bool                    m_bFullScreen;
    QSize                   m_WindowSize;
    bool                    m_bPluginLoad;
    QString                 m_strPrefix;
    QString                 m_strConfig;
    QWidget*                m_pRenderWidget;
    QStackedLayout*         m_pStackLayout;

    IMainFramePlugin*       m_pMainFramePlugin;
    ILoginPlugin*           m_pLoginPlugin;
    QList<IWidgetPlugin*>   m_PluginList;

    bool                    m_bMousePress;
    QPoint                  m_PressPoint;
    func_UninitializeEngine m_pUninitializeEngineFunc;

    QMap<QString,QList<WebCoverPos>> m_mapWebCoverPos;
    BCWebView*              m_pBCWebView;
    bool                    m_bCanKeyDown;
    QSystemTrayIcon*        m_trayIcon;
    QMap<QString,bool>      m_mDataChange;
    AppSetting*             m_pAppSetting;
    QMap<QWidget*,QString>  m_mapAllWidgets;

    int                     m_nLastWindowWidth;
    int                     m_nLastWindowHeight;

public slots:
    void slotFromWebFun(QJsonValue WebParam);
    void slotRunJSFromCore(QString strJson);
    void slotTrigerMenu(QAction* act);
    void slotKillProcess(QString strInfo,QString strTitle);
//    void downloadProgress(quint64 uiRequestId, qint64 iBytesDownload, qint64 iBytesTotal);

signals:
    void sigReloadWeb();
    void sigRunJsFromCore(QString strJson);
    void sigFromWebFun(QJsonValue WebParam);
    void sigKillProcess(QString strInfo,QString strTitle);
};

#endif // BCPLUGINMANAGER_H
