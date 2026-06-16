#ifndef MAINFRAME_H
#define MAINFRAME_H

#include <widget/bcmainframe.h>

class MainFrame : public BCMainFrame
{
public:
    MainFrame(IPluginManager *pPluginManager);
    virtual void CreateWidget();

    virtual ~MainFrame();
};

#endif // HCMAINFRAME_H
