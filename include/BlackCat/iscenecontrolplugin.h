#ifndef ISCENECONTROLPLUGIN_H
#define ISCENECONTROLPLUGIN_H
#include "iwidgetplugin.h"

class ISceneControlPlugin : public IWidgetPlugin
{
public:
    ISceneControlPlugin(QString objID) : IWidgetPlugin(SceneControlPluginType, objID) { }
    virtual void SceneLoad() = 0;
    virtual void iExecuteCommand(ICommand *pCommand)
    {
        switch(pCommand->eMessage)
        {
        case COMMAND_SCENE_LOAD:
        {
            SceneLoad();
        }
            break;
        case COMMAND_ITEM_CLICKED:
        {

        }
            break;
        case COMMAND_SELECT_NODE:
        {

        }
            break;
        default:
            break;
        }
    }
private:

};

QT_BEGIN_NAMESPACE

#define ISceneControlPlugin_iid "com.hc-project.Builder.ISceneControlPlugin"

Q_DECLARE_INTERFACE(ISceneControlPlugin, ISceneControlPlugin_iid)
QT_END_NAMESPACE
#endif // ISCENECONTROLPLUGIN_H
