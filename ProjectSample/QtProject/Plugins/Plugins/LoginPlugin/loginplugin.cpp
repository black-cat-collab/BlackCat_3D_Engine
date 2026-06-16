#include "loginplugin.h"
#include "logindialog.h"
#include <QDebug>

IPluginManager *g_pPluginManager = NULL;

LoginPlugin::LoginPlugin() :
    ILoginPlugin("LoginPlugin")
{

}

bool LoginPlugin::Init()
{
    g_pPluginManager = GetPluginManager();

    m_pLogin = new LoginDialog;

    return true;
}

bool LoginPlugin::Login()
{
    if(m_pLogin->exec() == QDialog::Accepted)
    {
        return true;
    }

    return false;

}
