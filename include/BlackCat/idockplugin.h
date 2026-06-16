#ifndef IDOCKPLUGIN_H
#define IDOCKPLUGIN_H

#include "iwidgetplugin.h"
#include <QDockWidget>

class IDockPlugin : public IWidgetPlugin
{
public:
    IDockPlugin(QString objID) : IWidgetPlugin(DockPluginType, objID),
        m_pDockWidget(nullptr){ }
    virtual QString Title() = 0;
    virtual Qt::DockWidgetArea Area() = 0;
    virtual int Index() { return 0; }
    virtual bool IsExtension() { return false; }
    QDockWidget* GetDockWidget() { return m_pDockWidget; }

    virtual void SetDockTitleWidget(QWidget *pTitleWidget)
    {
        if(m_pDockWidget)
        {
            m_pDockWidget->setTitleBarWidget(pTitleWidget);

        }
    }
protected:
    QDockWidget *m_pDockWidget;

private:

};

QT_BEGIN_NAMESPACE

#define IDockPlugin_iid "com.hc-project.Builder.IDockPlugin"

Q_DECLARE_INTERFACE(IDockPlugin, IDockPlugin_iid)
QT_END_NAMESPACE

#endif // IDOCKPLUGIN_H
