#-------------------------------------------------
#
# Project created by QtCreator 2019-11-06T17:54:06
#
#-------------------------------------------------

TARGET = BlackCat

 QT += widgets network webenginewidgets xml websockets printsupport

greaterThan(QT_MAJOR_VERSION, 6) | equals(QT_MAJOR_VERSION, 6) : CONFIG += bcqt6
bcqt6{
    QT += openglwidgets
    DEFINES += BCQT6
}else {
    QT += webengine
}

TEMPLATE = lib

DEFINES += BLACKCAT_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

contains(QT_ARCH, x86_64){
  #DEFINES += USE_NVIDIA_HARD_ENCODE    #if use nvidia hard encode
}
CONFIG += c++14

INCLUDEPATH += $$PWD/../../include


win32{

    DEPENDPATH += $$PWD/libs $$PWD/../lib/win_x64
    LIBS += opengl32.lib glu32.lib User32.Lib

    CONFIG(debug, release|debug){
        DESTDIR = $$PWD/../../lib/win_x64/Debug
        DLLDESTDIR = $$PWD/../../bin/Debug
        bcqt6{
            LIBS += -L$$PWD/libs/qBreakpad_qt6 qBreakpad_d.lib
        }else {
            LIBS += -L$$PWD/libs/qBreakpad qBreakpad_d.lib
        }

        target.path = $$PWD/../../bin/Debug/
        target.files += $$PWD/../../lib/win_x64/Debug/BlackCat.pdb
    }

    CONFIG(release, release|debug){
        DESTDIR = $$PWD/../../lib/win_x64/Release
        DLLDESTDIR = $$PWD/../../bin/Release
        bcqt6{
            LIBS += -L$$PWD/libs/qBreakpad_qt6 qBreakpad.lib
        }else {
            LIBS += -L$$PWD/libs/qBreakpad qBreakpad.lib
        }

        target.path = $$PWD/../../bin/Release/
        target.files += $$PWD/../../lib/win_x64/Release/BlackCat.pdb
    }


}

unix {
    CONFIG += plugin
    CONFIG += no_batch


    CONFIG(debug, release|debug){

        DESTDIR = $$PWD/../../Debug
        DEPENDPATH += $$PWD/libs $$PWD/../lib/linux_x64/Debug
        LIBS +=  -L$$PWD/../lib/linux_x64/Debug -lUtility -lqBreakpad
    }

    CONFIG(release, release|debug){

        DESTDIR = $$PWD/../../Release
        DEPENDPATH += $$PWD/libs $$PWD/../lib/linux_x64/Release
		
        LIBS +=  -L$$PWD/../lib/linux_x64/Release -lUtility -lqBreakpad
    }
  
    
    INCLUDEPATH += .
    
	

    QMAKE_CFLAGS += -fpermissive -lXtst -lX11 -gdwarf-4 -gstrict-dwarf
    QMAKE_CXXFLAGS += -fpermissive -lXtst -lX11 -gdwarf-4 -gstrict-dwarf




}


INSTALLS += target

QMAKE_CXXFLAGS_RELEASE = $$QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO
QMAKE_LFLAGS_RELEASE = $$QMAKE_LFLAGS_RELEASE_WITH_DEBUGINFO

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    autoupdate.cpp \
    network/source/classmemorytracer.cpp \
    network/source/dllmain.cpp \
    network/source/networkcommonrequest.cpp \
    network/source/networkdownloadrequest.cpp \
    network/source/networkmanager.cpp \
    network/source/networkmtdownloadrequest.cpp \
    network/source/networkreply.cpp \
    network/source/networkrequest.cpp \
    network/source/networkrunnable.cpp \
    network/source/networkuploadrequest.cpp \
    network/source/networkutility.cpp \
    opengl/glwidget.cpp \
    proxy/inputsimulator.cpp \
    proxy/serverhttp.cpp \
    proxy/servermanager.cpp \
    proxy/tcpserver.cpp \
    proxy/websockethandler.cpp \
    proxy/websockettransfer.cpp \
    tool/bcmessagebox.cpp \
    tool/qbreakpadutil.cpp \
    web/bcwebenginepage.cpp \
    web/bcwebview.cpp \
    widget/bcmainframe.cpp \
    widget/bcmainwindow.cpp \
    widget/docktitlewidget.cpp \
    bcpluginmanager.cpp

bcqt6{
    SOURCES += \
        autoupdate.cpp \
        moc/moc_glwidget.cpp \
        moc/moc_docktitlewidget.cpp \
        moc/moc_bcmainframe.cpp \
        moc/moc_bcmainwindow.cpp \
        moc/moc_bcmessagebox.cpp \
        moc/moc_bcpluginmanager.cpp \
        moc/moc_bcwebview.cpp
}

HEADERS += \
    ../../include/BlackCat/autoupdate.h \
    ../../include/BlackCat/basicdefine.h \
    ../../include/BlackCat/bcmessagebox.h \
    ../../include/BlackCat/bcpluginmanager.h \
    ../../include/BlackCat/bcresolutionwidget.h \
    ../../include/BlackCat/bcwebview.h \
    ../../include/BlackCat/blackcat_global.h \
    ../../include/BlackCat/idockplugin.h \
    ../../include/BlackCat/iloginplugin.h \
    ../../include/BlackCat/imainframeplugin.h \
    ../../include/BlackCat/imenuplugin.h \
    ../../include/BlackCat/ipluginmanager.h \
    ../../include/BlackCat/iscenecontrolplugin.h \
    ../../include/BlackCat/isceneplugin.h \
    ../../include/BlackCat/itoolplugin.h \
    ../../include/BlackCat/iwidgetplugin.h \
    ../../include/BlackCat/corecommon.h \
    ../../include/BlackCat/glwidget.h \
    ../../include/BlackCat/qBreakpad/qbreakpadutil.h \
    ../../include/BlackCat/utils.h \
    ../../include/BlackCat/widget/bcmainframe.h \
    ../../include/BlackCat/widget/bcmainwindow.h \
    ../../include/BlackCat/widget/docktitlewidget.h \
    network/ihcnetwork.h \
    network/source/Log4cplusWrapper.h \
    network/source/classmemorytracer.h \
    network/source/networkcommonrequest.h \
    network/source/networkdefs.h \
    network/source/networkdownloadrequest.h \
    network/source/networkevent.h \
    network/source/networkglobal.h \
    network/source/networkmanager.h \
    network/source/networkmtdownloadrequest.h \
    network/source/networkreply.h \
    network/source/networkrequest.h \
    network/source/networkrunnable.h \
    network/source/networkuploadrequest.h \
    network/source/networkutility.h \
    proxy/inputsimulator.h \
    proxy/serverhttp.h \
    proxy/servermanager.h \
    proxy/tcpserver.h \
    proxy/websockethandler.h \
    proxy/websockettransfer.h \
    web/bcwebenginepage.h

DISTFILES += \
    qtsingleapplication/INSTALL.TXT \
    qtsingleapplication/README.TXT \
    qtsingleapplication/qtsingleapplication.pri \
    qtsingleapplication/qtsinglecoreapplication.pri

RESOURCES += \
    resource.qrc


