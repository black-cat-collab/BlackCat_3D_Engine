#ifndef SCENECONTROLPLUGIN_H
#define SCENECONTROLPLUGIN_H

#include <QWidget>
#include <QObject>
#include <iscenecontrolplugin.h>
#include <imainframeplugin.h>
#include <corecommon.h>

class SceneControlPlugin;
class SceneControlWidget;

extern SceneControlPlugin *g_pSceneControlPlugin;
extern IPluginManager *g_pIPluginManager;
extern bc::ISystemAPI   *g_pSystemAPI;

class SceneControlPlugin : public QObject, public ISceneControlPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.hc-project.Builder.ISceneControlPlugin" FILE "SceneControlPlugin.json")
    Q_INTERFACES(ISceneControlPlugin)

public:
    SceneControlPlugin();
    virtual QWidget *CreateWidget(QWidget *parent = NULL);
    virtual QWidget *GetWidget();
    virtual void SceneLoad();

    virtual void iExecuteCommand(ICommand *pCommand);

private:
    void init();
private:
    SceneControlWidget *m_pSceneControlWidget;

};

#endif // SCENECONTROLPLUGIN_H
