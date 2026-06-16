#ifndef LOGINPLUGIN_H
#define LOGINPLUGIN_H

#include <QWidget>
#include <QObject>
#include <iloginplugin.h>
#include <corecommon.h>
#include <ipluginmanager.h>

extern IPluginManager *g_pPluginManager;

class LoginDialog;
class LoginPlugin : public QObject, public ILoginPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.hc-project.Builder.ILoginPlugin" FILE "LoginPlugin.json")
    Q_INTERFACES(ILoginPlugin)

public:
    LoginPlugin();

    bool Init();
    bool Login();

private:
    void init();
private:
    LoginDialog *m_pLogin;

};

#endif // LOGINPLUGIN_H
