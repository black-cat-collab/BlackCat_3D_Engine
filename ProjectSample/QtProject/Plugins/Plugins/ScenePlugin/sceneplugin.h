#ifndef DOCKPLUGIN_H
#define DOCKPLUGIN_H

#include <QWidget>
#include <QObject>
#include <isceneplugin.h>
#include <corecommon.h>
#include <glwidget.h>
#include <ipluginmanager.h>

extern IPluginManager *g_pPluginManager;
extern bc::ISystemAPI *g_pSystemAPI;

class SceneWidget;
class ScenePlugin : public QObject, public IScenePlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.hc-project.Builder.IScenePlugin" FILE "ScenePlugin.json")
    Q_INTERFACES(IScenePlugin)

public:
    ScenePlugin();
    virtual QWidget *CreateWidget(QWidget *parent = NULL);
    virtual QWidget *GetWidget();
};

#endif // DOCKPLUGIN_H
