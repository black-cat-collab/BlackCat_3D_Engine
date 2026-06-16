#ifndef IMENUPLUGIN_H
#define IMENUPLUGIN_H

#include "iwidgetplugin.h"

class IMenuPlugin : public IWidgetPlugin
{
public:
    IMenuPlugin(QString objID) : IWidgetPlugin(MenuPluginType, objID) { }
    virtual QMenu* FileMenu() = 0;
    virtual QMenu* EditMenu() = 0;
    virtual QMenu* ViewMenu() = 0;
    virtual QMenu* HelpMenu() = 0;
    virtual QMenu* PluginMenu() = 0;
    virtual QMenu* ToolsMenu() = 0;
protected:


private:

};

QT_BEGIN_NAMESPACE

#define IMenuPlugin_iid "com.hc-project.Builder.IMenuPlugin"

Q_DECLARE_INTERFACE(IMenuPlugin, IMenuPlugin_iid)
QT_END_NAMESPACE


#endif // IMENUPLUGIN_H
