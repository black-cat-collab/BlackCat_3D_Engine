#include "sceneplugin.h"
#include "scenewidget.h"

IPluginManager *g_pPluginManager = NULL;
bc::ISystemAPI *g_pSystemAPI = NULL;

ScenePlugin::ScenePlugin() :
    IScenePlugin("ScenePlugin")
{
    m_pSceneWidget = NULL;
}

QWidget *ScenePlugin::CreateWidget(QWidget *parent)
{
    g_pPluginManager = GetPluginManager();

    m_pSceneWidget = new SceneWidget(g_pPluginManager, parent);
    return m_pSceneWidget;
}

QWidget *ScenePlugin::GetWidget()
{
   return m_pSceneWidget;
}
