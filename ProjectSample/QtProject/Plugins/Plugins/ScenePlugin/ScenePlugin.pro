QT       += core gui widgets network

TARGET = ScenePlugin
TEMPLATE = lib
CONFIG += plugin

INCLUDEPATH += ../../../../../include ../../../../../include/BlackCat

greaterThan(QT_MAJOR_VERSION, 6) | equals(QT_MAJOR_VERSION, 6) : CONFIG += bcqt6
bcqt6{
    DEFINES += BCQT6
    QT += openglwidgets
}



win32{

    CONFIG(debug, release|debug){
        DEPENDPATH += ../../../../../lib/win_x64/Debug
        LIBS += -L$$PWD/../../../../../lib/win_x64/Debug -lBlackCat

        DESTDIR = $$PWD/../../../../Debug/plugins/scene
    }
    CONFIG(release, release|debug){
        DEPENDPATH += ../../../../../lib/win_x64/Release
        LIBS += -L$$PWD/../../../../../lib/win_x64/Release -lBlackCat

        DESTDIR = $$PWD/../../../../Release/plugins/scene
    }

}


unix{

        CONFIG(debug, release|debug){

           DESTDIR = $$PWD/../../../../../Debug/plugins/scene
        }

        CONFIG(release, release|debug){

           DESTDIR = $$PWD/../../../../../Release/plugins/scene
        }

}



TARGET          = $$qtLibraryTarget(projectsceneplugin)

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    sceneplugin.cpp \
    scenewidget.cpp

HEADERS += \
    sceneplugin.h \
    scenewidget.h

DISTFILES += ScenePlugin.json

