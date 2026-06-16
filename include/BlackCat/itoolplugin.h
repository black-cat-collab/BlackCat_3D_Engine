#ifndef ITOOLBARPLUGIN_H
#define ITOOLBARPLUGIN_H

#include "iwidgetplugin.h"
#include <QToolBar>

class IToolPlugin : public IWidgetPlugin
{
public:
    IToolPlugin(QString objID) : IWidgetPlugin(ToolPluginType, objID),
    m_pToolBar(nullptr) { }
    virtual Qt::ToolBarArea ToolBarArea() { return Qt::TopToolBarArea; }
    virtual QString Title() = 0;
    QToolBar* GetToolBar() { return m_pToolBar; }
protected:
    QToolBar*       m_pToolBar;

private:

};

QT_BEGIN_NAMESPACE

#define IToolPlugin_iid "com.bc-project.Builder.IToolPlugin"

Q_DECLARE_INTERFACE(IToolPlugin, IToolPlugin_iid)
QT_END_NAMESPACE

#endif // ITOOLBARPLUGIN_H
