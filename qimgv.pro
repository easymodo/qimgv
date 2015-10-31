#-------------------------------------------------
#
# Project created by QtCreator 2014-08-18T10:07:13
#
#-------------------------------------------------

QT       += core gui widgets concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets multimedia multimediawidgets

TARGET = qimgv
TEMPLATE = app
RC_FILE = qimgv.rc

QMAKE_CXXFLAGS += -Wall
QMAKE_CXXFLAGS += -std=c++11

SOURCES += main.cpp\
        mainwindow.cpp \
    overlays/infooverlay.cpp \
    fileinfo.cpp \
    overlays/controlsoverlay.cpp \
    sourceContainers/imagestatic.cpp \
    core.cpp \
    directorymanager.cpp \
    opendialog.cpp \
    imagecache.cpp \
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
    viewers/videoplayer.cpp \
    sourceContainers/video.cpp \
    loadhelper.cpp \
    newloader.cpp \
    imagefactory.cpp \
    thumbnailer.cpp \
    lib/stuff.cpp \
    wallpapersetter.cpp \
    actionmanager.cpp \
    customWidgets/settingsshortcutwidget.cpp \
    sourceContainers/clip.cpp \
    thumbnailPanel/thumbnailview.cpp \
    customWidgets/clickablewidget.cpp

HEADERS  += mainwindow.h \
    overlays/infooverlay.h \
    fileinfo.h \
    overlays/controlsoverlay.h \
    sourceContainers/imagestatic.h \
    core.h \
    directorymanager.h \
    opendialog.h \
    imagecache.h \
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
    viewers/videoplayer.h \
    sourceContainers/video.h \
    loadhelper.h \
    newloader.h \
    imagefactory.h \
    thumbnailer.h \
    wallpapersetter.h \
    lib/stuff.h \
    actionmanager.h \
    customWidgets/settingsshortcutwidget.h \
    sourceContainers/clip.h \
    thumbnailPanel/thumbnailview.h \
    customWidgets/clickablewidget.h

FORMS    += \
    settingsdialog.ui

RESOURCES += \
    resources.qrc
