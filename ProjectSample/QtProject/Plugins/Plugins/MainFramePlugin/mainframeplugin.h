#ifndef INTERNALMAINFRAMEPLUGIN_H
#define INTERNALMAINFRAMEPLUGIN_H

#include <QWidget>
#include <QObject>
#include <imainframeplugin.h>
#include <corecommon.h>
#include <QMenu>

class MainFramePlugin : public QObject, public IMainFramePlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.hc-project.Builder.IMainFramePlugin" FILE "MainFramePlugin.json")
    Q_INTERFACES(IMainFramePlugin)

public:
    MainFramePlugin();
    ~MainFramePlugin();
    CreateMainFrameStatus CreateFrame();
    void iShutDown();

};

#endif // INTERNALMAINFRAMEPLUGIN_H
