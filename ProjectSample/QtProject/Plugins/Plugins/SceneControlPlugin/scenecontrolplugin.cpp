#include "scenecontrolplugin.h"
#include "scenecontrolwidget.h"

bc::ISystemAPI* g_pSystemAPI      = NULL;
IPluginManager* g_pIPluginManager = NULL;
SceneControlPlugin* g_pSceneControlPlugin = NULL;

SceneControlPlugin::SceneControlPlugin() :
    ISceneControlPlugin("SceneControlPlugin"),
    m_pSceneControlWidget(NULL)
{
    g_pSceneControlPlugin = this;
}

QWidget *SceneControlPlugin::CreateWidget(QWidget *parent)
{
    g_pIPluginManager   = GetPluginManager();
    g_pSystemAPI  = g_pIPluginManager->iGetSystemAPI();

    if(g_pIPluginManager == NULL || g_pSystemAPI == NULL)
    {
        return NULL;
    }

    m_pSceneControlWidget  = new SceneControlWidget(GetPluginManager()->iGetWindowHeight(), parent);

    return m_pSceneControlWidget;
}


QWidget *SceneControlPlugin::GetWidget()
{
    return m_pSceneControlWidget;
}

void SceneControlPlugin::SceneLoad()
{
    if(m_pSceneControlWidget)
    {
        m_pSceneControlWidget->SceneLoad();
    }
}

void SceneControlPlugin::iExecuteCommand(ICommand *pCommand)
{
    ISceneControlPlugin::iExecuteCommand(pCommand);
    if(pCommand->eMessage == COMMAND_START_LOAD_WEB)
    {
        if(m_pSceneControlWidget)
        {
            m_pSceneControlWidget->StartLoadWebPage();
        }
    }
    else if(pCommand->eMessage == COMMAND_START_LOAD_SCENE)
    {
        if(m_pSceneControlWidget)
        {
        }
    }
}

