/********************************************************************************
** Form generated from reading UI file 'scenecontrolwidget.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SCENECONTROLWIDGET_H
#define UI_SCENECONTROLWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SceneControlWidget
{
public:

    void setupUi(QWidget *SceneControlWidget)
    {
        if (SceneControlWidget->objectName().isEmpty())
            SceneControlWidget->setObjectName(QString::fromUtf8("SceneControlWidget"));
        SceneControlWidget->resize(1920, 1080);
        SceneControlWidget->setStyleSheet(QString::fromUtf8(""));

        retranslateUi(SceneControlWidget);

        QMetaObject::connectSlotsByName(SceneControlWidget);
    } // setupUi

    void retranslateUi(QWidget *SceneControlWidget)
    {
        SceneControlWidget->setWindowTitle(QCoreApplication::translate("SceneControlWidget", "Form", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SceneControlWidget: public Ui_SceneControlWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SCENECONTROLWIDGET_H
