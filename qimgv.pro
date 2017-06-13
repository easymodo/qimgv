#-------------------------------------------------
#
# Project created by QtCreator 2014-08-18T10:07:13
#
#-------------------------------------------------

QT       += core gui widgets concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

msvc: LIBS += -luser32

TARGET = qimgv
TEMPLATE = app
RC_FILE = qimgv.rc

QMAKE_CXXFLAGS += -Wall
QMAKE_CXXFLAGS += -std=c++11
QMAKE_CXXFLAGS_RELEASE -= -O
QMAKE_CXXFLAGS_RELEASE -= -O1
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE *= -O3

unix {
    QT_CONFIG -= no-pkg-config
    CONFIG += link_pkgconfig
    PKGCONFIG += mpv
}

SOURCES += main.cpp\
    mainwindow.cpp \
    overlays/infooverlay.cpp \
    fileinfo.cpp \
    overlays/controlsoverlay.cpp \
    sourceContainers/imagestatic.cpp \
    core.cpp \
    directorymanager.cpp \
    opendialog.cpp \
    viewers/imageviewer.cpp \
    sleep.cpp \
    settings.cpp \
    settingsdialog.cpp \
    lib/imagelib.cpp \
    overlays/mapoverlay.cpp \
    overlays/cropoverlay.cpp \
    thumbnailPanel/thumbnailstrip.cpp \
    thumbnailPanel/thumbnaillabel.cpp \
    sourceContainers/image.cpp \
    sourceContainers/imageanimated.cpp \
    customWidgets/clickablelabel.cpp \
    sourceContainers/thumbnail.cpp \
    sourceContainers/video.cpp \
    imagefactory.cpp \
    thumbnailer.cpp \
    lib/stuff.cpp \
    wallpapersetter.cpp \
    actionmanager.cpp \
    customWidgets/settingsshortcutwidget.cpp \
    sourceContainers/clip.cpp \
    thumbnailPanel/thumbnailview.cpp \
    customWidgets/clickablewidget.cpp \
    resizedialog.cpp \
    filesystemWatchers/watcherwindows.cpp \
    customWidgets/slidevpanel.cpp \
    customWidgets/slidehpanel.cpp \
    customWidgets/slidepanel.cpp \
    customWidgets/toolbox.cpp \
    loaderrunnable.cpp \
    loader.cpp \
    cache.cpp \
    viewers/videoplayergl.cpp \
    viewers/mpvwidget.cpp \
    core2.cpp \
    mainwindow2.cpp \
    viewerwidget.cpp \
    customWidgets/mainpanel.cpp \
    customWidgets/iconbutton.cpp \
    thumbnailcache.cpp \
    customWidgets/overlaywidget.cpp \
    components/scaler/scaler.cpp \
    components/scaler/scalerrunnable.cpp

HEADERS += mainwindow.h \
    overlays/infooverlay.h \
    fileinfo.h \
    overlays/controlsoverlay.h \
    sourceContainers/imagestatic.h \
    core.h \
    directorymanager.h \
    opendialog.h \
    viewers/imageviewer.h \
    settings.h \
    settingsdialog.h \
    lib/imagelib.h \
    overlays/mapoverlay.h \
    overlays/cropoverlay.h \
    thumbnailPanel/thumbnailstrip.h \
    thumbnailPanel/thumbnaillabel.h \
    sourceContainers/image.h \
    sourceContainers/imageanimated.h \
    customWidgets/clickablelabel.h \
    sourceContainers/thumbnail.h \
    sourceContainers/video.h \
    imagefactory.h \
    thumbnailer.h \
    wallpapersetter.h \
    lib/stuff.h \
    actionmanager.h \
    customWidgets/settingsshortcutwidget.h \
    sourceContainers/clip.h \
    thumbnailPanel/thumbnailview.h \
    customWidgets/clickablewidget.h \
    resizedialog.h \
    filesystemWatchers/watcherwindows.h \
    customWidgets/slidevpanel.h \
    customWidgets/slidehpanel.h \
    customWidgets/slidepanel.h \
    customWidgets/toolbox.h \
    loaderrunnable.h \
    loader.h \
    cache.h \
    viewers/videoplayergl.h \
    viewers/mpvwidget.h \
    core2.h \
    mainwindow2.h \
    viewerwidget.h \
    customWidgets/mainpanel.h \
    customWidgets/iconbutton.h \
    thumbnailcache.h \
    customWidgets/overlaywidget.h \
    components/scaler/scaler.h \
    components/scaler/scalerrequest.h \
    components/scaler/scalerrunnable.h

FORMS += \
    settingsdialog.ui

RESOURCES += \
    resources.qrc
