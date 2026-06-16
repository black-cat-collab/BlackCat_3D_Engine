QT       += core gui widgets xml network webenginewidgets

TARGET = MainFramePlugin
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

          DESTDIR = $$PWD/../../../../Debug/plugins/mainframe

    }
    CONFIG(release, release|debug){
           DEPENDPATH += $$PWD/libs  ../../../../../lib/win_x64/Release

           LIBS += -L$$PWD/../../../../../lib/win_x64/Release -lBlackCat

           DESTDIR = $$PWD/../../../../Release/plugins/mainframe
    }

    LIBS += User32.Lib


}
unix {

        CONFIG(debug, release|debug){

           LIBS += -L$$PWD/../../../../../Debug -lBlackCat
           DESTDIR = $$PWD/../../../../../Debug/plugins/mainframe
        }

        CONFIG(release, release|debug){

           LIBS += -L$$PWD/../../../../../Release -lBlackCat
           DESTDIR = $$PWD/../../../../../Release/plugins/mainframe
        }



}


TARGET = $$qtLibraryTarget(projectmainframeplugin)

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    mainframeplugin.cpp \
    mainwindow.cpp \
    mainframe.cpp

HEADERS += \
    mainframeplugin.h \
    mainwindow.h \
    mainframe.h

DISTFILES += MainFramePlugin.json


