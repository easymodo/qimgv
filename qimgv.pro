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

SOURCES += main.cpp\
        mainwindow.cpp \
    infooverlay.cpp \
    fileinfo.cpp \
    controlsoverlay.cpp \
    mapoverlay.cpp \
    image.cpp \
    core.cpp \
    directorymanager.cpp \
    imageloader.cpp \
    opendialog.cpp \
    imagecache.cpp \
    imageviewer.cpp \
    sleep.cpp \
    settings.cpp \
    settingsdialog.cpp

HEADERS  += mainwindow.h \
    infooverlay.h \
    fileinfo.h \
    controlsoverlay.h \
    mapoverlay.h \
    image.h \
    core.h \
    directorymanager.h \
    imageloader.h \
    opendialog.h \
    imagecache.h \
    imageviewer.h \
    settings.h \
    settingsdialog.h

FORMS    += \
    settingsdialog.ui

RESOURCES += \
    resources.qrc
