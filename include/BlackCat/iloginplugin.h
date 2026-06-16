#ifndef ILOGINPLUGIN_H
#define ILOGINPLUGIN_H

#include "iwidgetplugin.h"

class ILoginPlugin : public IWidgetPlugin
{
public:
    ILoginPlugin(QString objID) : IWidgetPlugin(LoginPluginType, objID) { }
    virtual bool Init() = 0;
    virtual bool Login() = 0;
protected:


private:

};

QT_BEGIN_NAMESPACE

#define ILoginPlugin_iid "com.hc-project.Builder.ILoginPlugin"

Q_DECLARE_INTERFACE(ILoginPlugin, ILoginPlugin_iid)
QT_END_NAMESPACE

#endif // ILOGINPLUGIN_H
