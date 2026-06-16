#include "scenewidget.h"
#include "sceneplugin.h"

using namespace bc;

SceneWidget::SceneWidget(IPluginManager* pPluginManager, QWidget *parent)
    : GLWidget(pPluginManager, parent)
{
    m_nSwapInterval = -1;
}

SceneWidget::~SceneWidget()
{

}
