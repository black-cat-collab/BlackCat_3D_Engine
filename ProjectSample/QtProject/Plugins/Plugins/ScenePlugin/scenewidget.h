#ifndef __SCENEWIDGET_H__
#define __SCENEWIDGET_H__

#include <corecommon.h>
#include <QOpenGLWidget>
#include <QtWidgets>
#include <QOpenGLFunctions>
#include <QColor>
#include <QProgressBar>
#include <glwidget.h>

class SceneWidget : public GLWidget
{
    Q_OBJECT

public:
    explicit SceneWidget(IPluginManager *pPluginManager, QWidget *parent = NULL);
    ~SceneWidget();
};

#endif   //__SCENEWIDGET_H__
