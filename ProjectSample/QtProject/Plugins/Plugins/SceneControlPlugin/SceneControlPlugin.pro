QT       += core gui widgets webenginewidgets xml

TARGET = SceneControlPlugin
TEMPLATE = lib
CONFIG += plugin

INCLUDEPATH += ../../../../../include ../../../../../include/BlackCat

greaterThan(QT_MAJOR_VERSION, 6) | equals(QT_MAJOR_VERSION, 6) : CONFIG += bcqt6
bcqt6{
    DEFINES += BCQT6
}


win32{
    CONFIG(debug, release|debug){
        DEPENDPATH += $$PWD/libs  ../../../../../lib/win_x64/Debug
        LIBS += -L$$PWD/../../../../../lib/win_x64/Debug -lBlackCat

        DESTDIR = $$PWD/../../../../Debug/plugins/scenecontrol
    }
    CONFIG(release, release|debug){

        DEPENDPATH += $$PWD/libs  ../../../../../lib/win_x64/Release
        LIBS += -L$$PWD/../../../../../lib/win_x64/Release -lBlackCat

        DESTDIR = $$PWD/../../../../Release/plugins/scenecontrol
    }

}

unix{

        CONFIG(debug, release|debug){

           DESTDIR = $$PWD/../../../../../Debug/plugins/scenecontrol
        }

        CONFIG(release, release|debug){

           DESTDIR = $$PWD/../../../../../Release/plugins/scenecontrol
        }


}
TARGET = $$qtLibraryTarget(projectscenecontrolplugin)

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    scenecontrolplugin.cpp \
    scenecontrolwidget.cpp \
    Project/projectmanager.cpp

HEADERS += \
    scenecontrolplugin.h \
    scenecontrolwidget.h \
    Project/projectmanager.h

DISTFILES += SceneControlPlugin.json

FORMS += \
    scenecontrolwidget.ui

