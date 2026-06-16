#include "pluginmanagerclient.h"

PluginManagerClient* PluginManagerClient::m_pInstance = NULL;

PluginManagerClient::PluginManagerClient() :
    BCPluginManager(),
    //m_pUninitializeProjectCoreFunc(NULL),
    m_pUninitializeProjectClientFunc(NULL)
{
    m_bAutoLogin = false;
}

PluginManagerClient::~PluginManagerClient()
{
    //if(m_pUninitializeProjectCoreFunc)
    //{
    //    m_pUninitializeProjectCoreFunc(m_pSystemAPI);
    //}

    if(m_pUninitializeProjectClientFunc)
    {
        m_pUninitializeProjectClientFunc(m_pSystemAPI);
    }
}

PluginManagerClient* PluginManagerClient::GetInstance()
{
    if(m_pInstance == NULL)
    {
        m_pInstance = new PluginManagerClient;
    }

    return m_pInstance;
}

void PluginManagerClient::Destroy()
{
    qDebug()<<"close";

    delete m_pInstance;
}

bool PluginManagerClient::loadProjectDll()
{
    QString strClientDLL = QString::fromStdString(m_pSystemAPI->iEngineAPI->iGetProjectConfig().sBootConfig.strClientDLL);
    if(!strClientDLL.isEmpty())
    {
#ifndef __GNUC__
        QLibrary kee3dLib(strClientDLL+".dll");
#else
        QLibrary kee3dLib("lib"+strClientDLL+".so");
#endif

        func_InitializeAPIProjectClient pInitFunc = (func_InitializeAPIProjectClient)kee3dLib.resolve("InitializeAPIProjectClient");
        if(!pInitFunc)
        {
            return false;
        }

        m_pUninitializeProjectClientFunc = (func_UninitializeAPIProjectClient)kee3dLib.resolve("UninitializeAPIProjectClient");

        bc::IProjectClientAPI* pProjectClientAPI = pInitFunc(m_pSystemAPI);
        if(pProjectClientAPI == NULL)
        {
            return false;
        }
    }


    return true;
}

void PluginManagerClient::iAddWebClientBrowser(QWidget *pParent, QString strUrl)
{
    if(m_bAutoLogin)
    {
        if(strUrl.contains("?"))
        {
            strUrl += "&isAutoLogin=1";
        }
        else
        {
            strUrl += "?isAutoLogin=1";
        }
    }
    BCPluginManager::iAddWebClientBrowser(pParent,strUrl);
}
