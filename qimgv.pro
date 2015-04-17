#-------------------------------------------------
#
# Project created by QtCreator 2014-08-18T10:07:13
#
#-------------------------------------------------

QT       += core gui widgets concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qimgv
TEMPLATE = app
RC_FILE = qimgv.rc

QMAKE_CXXFLAGS += -Wall
QMAKE_CXXFLAGS += -std=c++11

SOURCES += main.cpp\
        mainwindow.cpp \
    infooverlay.cpp \
    fileinfo.cpp \
    controlsoverlay.cpp \
    image.cpp \
    core.cpp \
    directorymanager.cpp \
    imageloader.cpp \
    opendialog.cpp \
    imagecache.cpp \
    imageviewer.cpp \
    sleep.cpp \
    settings.cpp \
    settingsdialog.cpp \
    lib/imagelib.cpp \
    mapoverlay/mapoverlay.cpp \
    cropoverlay.cpp \
    thumbnailstrip/thumbnailstrip.cpp \
    thumbnailstrip/thumbnailitem.cpp

HEADERS  += mainwindow.h \
    infooverlay.h \
    fileinfo.h \
    controlsoverlay.h \
    image.h \
    core.h \
    directorymanager.h \
    imageloader.h \
    opendialog.h \
    imagecache.h \
    imageviewer.h \
    settings.h \
    settingsdialog.h \
    lib/imagelib.h \
    mapoverlay/mapoverlay.h \
    cropoverlay.h \
    thumbnailstrip/thumbnailstrip.h \
    thumbnailstrip/thumbnailitem.h

FORMS    += \
    settingsdialog.ui

RESOURCES += \
    resources.qrc
