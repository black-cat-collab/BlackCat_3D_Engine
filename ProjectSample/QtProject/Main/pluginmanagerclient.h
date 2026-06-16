#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include <QObject>
#include <QDockWidget>
#include <QMultiMap>
#include <QStackedLayout>
#include <ipluginmanager.h>
#include <bcpluginmanager.h>

//typedef bc::IProjectCoreAPI* (*func_InitializeAPIProjectCore)(bc::ISystemAPI* pSystemAPI);
//typedef void (*func_UninitializeAPIProjectCore)(bc::ISystemAPI* pSystemAPI);

typedef bc::IProjectClientAPI* (*func_InitializeAPIProjectClient)(bc::ISystemAPI* pSystemAPI);
typedef void (*func_UninitializeAPIProjectClient)(bc::ISystemAPI* pSystemAPI);

class PluginManagerClient : public BCPluginManager
{
    Q_OBJECT
public:
    static PluginManagerClient *GetInstance();
    static void Destroy();
    virtual ~PluginManagerClient();

protected:
    PluginManagerClient();
    virtual bool loadProjectDll();

    virtual void iAddWebClientBrowser(QWidget *pParent, QString strUrl);
protected:
    static PluginManagerClient*         m_pInstance;
    //func_UninitializeAPIProjectCore     m_pUninitializeProjectCoreFunc;
    func_UninitializeAPIProjectClient   m_pUninitializeProjectClientFunc;

public:
    bool m_bAutoLogin;

};

#endif // PLUGINMANAGER_H
