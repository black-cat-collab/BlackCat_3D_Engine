#ifndef ISCENEPLUGIN_H
#define ISCENEPLUGIN_H

#include <QCoreApplication>
#include <QMouseEvent>
#include "iwidgetplugin.h"
#include "glwidget.h"

class IScenePlugin : public IWidgetPlugin
{
public:
    IScenePlugin(QString objID) : IWidgetPlugin(ScenePluginType, objID), m_pSceneWidget(NULL) { }
    virtual void MouseDoubleClick(int button, QPoint pos)
    {
        RETURN_IF_PTR_IS_NULL(m_pSceneWidget);
        m_pSceneWidget->MouseDoubleClick(button, pos);
    }
    virtual void MousePress(int button, QPoint pos)
    {
        RETURN_IF_PTR_IS_NULL(m_pSceneWidget);
        m_pSceneWidget->MousePress(button, pos);
    }
    virtual void MouseRelease(int button, QPoint pos)
    {
        RETURN_IF_PTR_IS_NULL(m_pSceneWidget);
        m_pSceneWidget->MouseRelease(button, pos);
    }
    virtual void MouseMove(int button, QPoint pos)
    {
        RETURN_IF_PTR_IS_NULL(m_pSceneWidget);
        m_pSceneWidget->MouseMove(button, pos);
    }
    virtual void Wheel(QPoint pos, QPoint angleDelta)
    {
        RETURN_IF_PTR_IS_NULL(m_pSceneWidget);
        m_pSceneWidget->Wheel(pos, angleDelta);
    }
    virtual void KeyPress(int QtKey)
    {
        RETURN_IF_PTR_IS_NULL(m_pSceneWidget);
        m_pSceneWidget->KeyPress(QtKey);
    }
    virtual void KeyRelease(int QtKey)
    {
        RETURN_IF_PTR_IS_NULL(m_pSceneWidget);
        m_pSceneWidget->KeyRelease(QtKey);
    }
    virtual void SetLoad(bool bLoad)
    {
        RETURN_IF_PTR_IS_NULL(m_pSceneWidget);
        m_pSceneWidget->SetLoad(bLoad);
    }
    virtual void SetOpen(bool bOpen)
    {
        RETURN_IF_PTR_IS_NULL(m_pSceneWidget);
        m_pSceneWidget->SetOpen(bOpen);
    }
    virtual bool IsLoad()
    {
        RETURN_FALSE_IF_PTR_IS_NULL(m_pSceneWidget);
        return m_pSceneWidget->IsLoad();
    }
    virtual bool IsOpen()
    {
        RETURN_FALSE_IF_PTR_IS_NULL(m_pSceneWidget);
        return m_pSceneWidget->IsOpen();
    }
    virtual void ResizeGLWidget(int nWidth,int nHeight)
    {
        RETURN_IF_PTR_IS_NULL(m_pSceneWidget);
        m_pSceneWidget->ResizeGLWidget(nWidth,nHeight);
    }
    virtual void iExecuteCommand(ICommand *pCommand)
    {
        switch(pCommand->eMessage)
        {
        case COMMAND_MOUSE_DBCLICK:
        {
            MouseDoubleClick((int)pCommand->nParam1, QPoint((int)pCommand->nParam2, (int)pCommand->nParam3));
        }
            break;
        case COMMAND_MOUSE_PRESS:
        {
            MousePress((int)pCommand->nParam1, QPoint((int)pCommand->nParam2, (int)pCommand->nParam3));
        }
            break;
        case COMMAND_MOUSE_RELEASE:
        {
            MouseRelease((int)pCommand->nParam1, QPoint((int)pCommand->nParam2, (int)pCommand->nParam3));
        }
            break;
        case COMMAND_MOUSE_MOVE:
        {
            MouseMove((int)pCommand->nParam1, QPoint((int)pCommand->nParam2, (int)pCommand->nParam3));
        }
            break;
        case COMMAND_MOUSE_WHEEL:
        {
            Wheel(QPoint((int)pCommand->nParam1, (int)pCommand->nParam2), QPoint((int)pCommand->nParam3, (int)pCommand->nParam4));
        }
            break;
        case COMMAND_KEY_PRESS:
        {
            KeyPress((int)pCommand->nParam1);
        }
            break;
        case COMMAND_KEY_RELEASE:
        {
            KeyRelease((int)pCommand->nParam1);
        }
            break;
        case COMMAND_OPEN_SCENE:
        {
            SetOpen(true);
        }
            break;
        case COMMAND_SCENE_LOAD:
        {
            SetLoad(true);
        }
            break;
        case COMMAND_RESIZE_GLWIDGET:
        {
            ResizeGLWidget((int)pCommand->nParam1,(int)pCommand->nParam2);
        }
            break;
        default:
            break;
        }
    }

protected:
    GLWidget *m_pSceneWidget;
};

QT_BEGIN_NAMESPACE

#define IScenePlugin_iid "com.hc-project.Builder.IScenePlugin"

Q_DECLARE_INTERFACE(IScenePlugin, IScenePlugin_iid)
QT_END_NAMESPACE

#endif // ISCENEPLUGIN_H
