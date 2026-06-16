#ifndef IWIDGETPLUGIN_H
#define IWIDGETPLUGIN_H

#include <qglobal.h>
#include <QWidget>
#include "corecommon.h"
#include "basicdefine.h"
#include "ipluginmanager.h"

class IWidgetPlugin
{
public:
    IWidgetPlugin(PluginType type, QString objID) : m_ObjID(objID),
        m_PluginType(type), m_pPluginManager(NULL)
	{

	}

    virtual ~IWidgetPlugin() {}
    virtual QWidget *CreateWidget(QWidget *) { return NULL; }
    virtual QList<QWidget*> CreateWidgets(QWidget *) { return QList<QWidget*>(); }
    virtual QWidget *GetWidget() { return NULL; }

    virtual const QString& GetObjID() { return m_ObjID; }

    virtual const PluginType& GetPluginType() { return m_PluginType; }

    bc::EventReturnType_e iProcessEvent(const bc::BCEvent&) { return bc::EventReturnType_e::NONE; }

    virtual void iExecuteCommand(ICommand *) {}

    virtual int iGetExecuteSort() const { return 0; }

    void SetPluginManager(IPluginManager *pPluginManager)
    {
        m_pPluginManager = pPluginManager;
    }

    IPluginManager* GetPluginManager()
    {
        return m_pPluginManager;
    }

private:
    IPluginManager* m_pPluginManager;
    QString         m_ObjID;
    PluginType      m_PluginType;
};

QT_BEGIN_NAMESPACE

#define IWidgetPlugin_iid "com.hc-project.Builder.IWidgetPlugin"

Q_DECLARE_INTERFACE(IWidgetPlugin, IWidgetPlugin_iid)
QT_END_NAMESPACE

#endif // IWIDGETPLUGIN_H
