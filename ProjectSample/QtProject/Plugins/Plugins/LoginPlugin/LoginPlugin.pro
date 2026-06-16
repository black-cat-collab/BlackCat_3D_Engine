QT       += core gui widgets

TARGET = LoginPlugin
TEMPLATE = lib
CONFIG += plugin

INCLUDEPATH += ../../../../../include ../../../../../include/BlackCat

greaterThan(QT_MAJOR_VERSION, 6) | equals(QT_MAJOR_VERSION, 6) : CONFIG += bcqt6
bcqt6{
    DEFINES += BCQT6
}

win32{

    CONFIG(debug, release|debug){
       DEPENDPATH += ../../../../../lib/win_x64/Debug
       DESTDIR = $$PWD/../../../../Debug/plugins/login
    }
    CONFIG(release, release|debug){
       DEPENDPATH += ../../../../../lib/win_x64/Release

       DESTDIR = $$PWD/../../../../Release/plugins/login
    }
}

unix {
    
    CONFIG(debug, release|debug){
       DESTDIR = $$PWD/../../../../../Debug/plugins/login
    }

    CONFIG(release, release|debug){
       DESTDIR = $$PWD/../../../../../Release/plugins/login
    }

}

TARGET = $$qtLibraryTarget(projectloginplugin)

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    loginplugin.cpp \
    logindialog.cpp

HEADERS += \
    loginplugin.h \
    logindialog.h

DISTFILES += LoginPlugin.json

FORMS += \
    logindialog.ui
