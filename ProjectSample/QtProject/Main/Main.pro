#-------------------------------------------------
#
# Project created by QtCreator 2017-11-22T15:14:03
#
#-------------------------------------------------


QT       += core gui opengl xml uitools webenginewidgets network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TestDemo
TEMPLATE = app

CONFIG += c++14

greaterThan(QT_MAJOR_VERSION, 6) | equals(QT_MAJOR_VERSION, 6) : CONFIG += bcqt6
bcqt6{
    DEFINES += BCQT6
}

INCLUDEPATH += $$PWD/../../../include/BlackCat $$PWD/../../../include
win32 {
	CONFIG(debug, release|debug){
            DEPENDPATH += $$PWD/../../../lib/win_x64/Debug
            LIBS += -L$$PWD/../../../lib/win_x64/Debug BlackCat.lib
	    
	    DESTDIR = $$PWD/../../Debug/
	    
	}
	CONFIG(release, release|debug){
            DEPENDPATH += $$PWD/../../../lib/win_x64/Release
            LIBS += -L$$PWD/../../../lib/win_x64/Release BlackCat.lib

            DESTDIR = $$PWD/../../Release/
	}

}

unix {
        QMAKE_LFLAGS += -no-pie
		
        CONFIG(debug, release|debug) {

            DESTDIR = $$PWD/../../../Debug/
            LIBS += -L$$PWD/../../../Debug -lBlackCat

        }
        CONFIG(release, release|debug){
            DESTDIR = $$PWD/../../../Release/
            LIBS += -L$$PWD/../../../Release -lBlackCat
        }


}


# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
    main.cpp \
    pluginmanagerclient.cpp \
    qtsingleapplication/qtlocalpeer.cpp \
    qtsingleapplication/qtlockedfile.cpp \
    qtsingleapplication/qtlockedfile_unix.cpp \
    qtsingleapplication/qtlockedfile_win.cpp \
    qtsingleapplication/qtsingleapplication.cpp \
    qtsingleapplication/qtsinglecoreapplication.cpp

RC_FILE += \
    bc.rc

HEADERS += \
    Include/projectheader.h \
    pluginmanagerclient.h \
    qtsingleapplication/QtLockedFile \
    qtsingleapplication/QtSingleApplication \
    qtsingleapplication/qtlocalpeer.h \
    qtsingleapplication/qtlockedfile.h \
    qtsingleapplication/qtsingleapplication.h \
    qtsingleapplication/qtsinglecoreapplication.h

RESOURCES += \
    Resource.qrc

DISTFILES += \
    bc.rc \
    qtsingleapplication/qtsingleapplication.pri \
    qtsingleapplication/qtsinglecoreapplication.pri
