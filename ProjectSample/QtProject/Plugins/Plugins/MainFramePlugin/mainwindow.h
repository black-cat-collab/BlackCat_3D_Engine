#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QDoubleSpinBox>
#include <QUndoStack>
#include <QUndoCommand>
#include <QAction>
#include <QEvent>
#include <widget/bcmainwindow.h>

class MainWindow : public BCMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(IPluginManager *pPluginManager, QWidget *parent = 0);
    virtual ~MainWindow();
    virtual void InitWidget();
};

#endif // MAINWINDOW_H
