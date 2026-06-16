#ifndef IPLUGINMANANGER_H
#define IPLUGINMANANGER_H

#include "blackcat_global.h"
#include "corecommon.h"
#include <QSize>
#include <QWidget>

class IWidgetPlugin;

enum PluginType
{
    ScenePluginType = 0,
    MenuPluginType,
    ToolPluginType,
    DockPluginType,
    MainFramePluginType,
    SceneControlPluginType,
    LoginPluginType,
    MainGraphicsViewPluginType
};

enum PluginCommand
{
    /*signal app msg*/
    COMMAND_SIGNAL_APP_RESTORE = 0,
    COMMAND_SIGNAL_APP_CLOSE,
    /*application msg*/
    COMMAND_APP_CLOSE,
    COMMAND_APP_SHOW,
    COMMAND_APP_MIN,
    COMMAND_APP_MAX,
    COMMAND_APP_FULL_SCREEN,
    COMMAND_APP_SHOW_STATUSBAR,
    /*mouse msg*/
    COMMAND_MOUSE_DBCLICK,
    COMMAND_MOUSE_PRESS,
    COMMAND_MOUSE_RELEASE,
    COMMAND_MOUSE_MOVE,
    COMMAND_MOUSE_WHEEL,
    /*keyboard msg*/
    COMMAND_KEY_PRESS,
    COMMAND_KEY_RELEASE,
    /*scene msg*/
    COMMAND_RESIZE_GLWIDGET,
    COMMAND_OPEN_SCENE,
    COMMAND_SCENE_FILE_LOAD,
    COMMAND_SCENE_LOAD,
    COMMAND_PRE_FRAME_UPDATE,
    COMMAND_POST_FRAME_UPDATE,
    COMMAND_LOAD_DATA,
    COMMAND_SCENE_SWITCHED,
    COMMAND_ITEM_CLICKED,
    COMMAND_SELECT_NODE,
    COMMAND_DELETE_NODE,
    COMMAND_UPDATE_SCENE_TREE,
    COMMAND_VIEW_CHANGE,
    COMMAND_SELECT_LAYER,
    COMMAND_NEW_LAYER,
    COMMAND_RETURN_LAYER,
    COMMAND_UPDATE_LAYER,
    COMMAND_NODE_TO_LAYER,
    COMMAND_REMOVE_NODE_FROM_LAYER,
    COMMAND_RELOAD_WEB,
    COMMAND_START_LOAD_WEB,
    COMMAND_START_LOAD_SCENE,
    COMMAND_PLUGIN_LOAD,
    COMMAND_END
};

enum CreateMainFrameStatus
{
    CreateSuccess   = 0,
    LoadEngineError,
    CreateConfigError
};

class ICommand
{
public:
    int eMessage;
    INT_PTR nParam1;
    INT_PTR nParam2;
    INT_PTR nParam3;
    INT_PTR nParam4;
    QList<INT_PTR> dataList;
    QString strParam;
    ICommand():eMessage(0),nParam1(0),nParam2(0),nParam3(0),nParam4(0)
    {

    }

    ICommand(int e, INT_PTR n1 = 0, INT_PTR n2 = 0, INT_PTR n3 = 0, INT_PTR n4 = 0)
    {
        SetParams(e,n1,n2,n3,n4);
    }

    void SetParams(int e=0, INT_PTR n1=0, INT_PTR n2=0, INT_PTR n3=0, INT_PTR n4=0)
    {
        eMessage = e;
        nParam1 = n1;
        nParam2 = n2;
        nParam3 =  n3;
        nParam4 =  n4;
    }

    void SetDataList(QList<INT_PTR> list)
    {
        dataList = list;
    }

    void SetString(QString str)
    {
        strParam = str;
    }
};

class AppSetting
{
public:
    AppSetting()
    {

    }
    virtual ~AppSetting()
    {

    }
};

class IPluginManager
{
public:
    virtual void    iSetPluginPrefix(QString prefix) = 0;
    virtual void    iSetConfig(QString config) = 0;

    virtual bool    iLogin() = 0;
    virtual int     iLoadPlugin() = 0;
    virtual bool    iIsPluginLoad() = 0;
    virtual IWidgetPlugin* iGetPlugin(PluginType type) = 0;
    virtual QList<IWidgetPlugin*> iGetPlugins(PluginType type) = 0;


    virtual void    iMouseMove() = 0;
    virtual void    iMouseRelease(int x, int y) = 0;
    virtual void    iMousePress(int x, int y) = 0;

    virtual void    iSetWindowSize(QSize size) = 0;
    virtual void    iSetWindowSize(float fWidth, float fHeight)   = 0;
    virtual float   iGetWindowWidth() = 0;
    virtual float   iGetWindowHeight() = 0;

    virtual void    iSendCommand(IWidgetPlugin *pSender, ICommand *pCommand) = 0;

    virtual void    iStartGame() = 0;

    virtual void    iSetDataChange(bool bChange, QString strID = "") = 0;
    virtual bool    iIsDataChange(QString strID = "") = 0;

    virtual bc::ISystemAPI* iGetSystemAPI() = 0;
    virtual void    iAddWebClientBrowser(QWidget *pParent, QString strUrl) = 0;
    virtual bool    iGetCanKeyEvent() = 0;
    virtual bool    iCheckWebCanHandleScene(QPoint &point) = 0;
    virtual AppSetting* iGetAppSetting() = 0;
};

#endif // IPLUGINMANANGER_H
