#-------------------------------------------------
#
# Project created by QtCreator 2014-08-18T10:07:13
#
#-------------------------------------------------

QT       += core gui widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qimgv
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    infooverlay.cpp \
    fileinfo.cpp \
    controlsoverlay.cpp \
    mapoverlay.cpp \
    image.cpp \
    imageviewer.cpp \
    scrollarea.cpp \
    core.cpp \
    directorymanager.cpp \
    imageloader.cpp

HEADERS  += mainwindow.h \
    infooverlay.h \
    fileinfo.h \
    controlsoverlay.h \
    mapoverlay.h \
    image.h \
    imageviewer.h \
    scrollarea.h \
    core.h \
    directorymanager.h \
    imageloader.h

FORMS    +=

RESOURCES += \
    resources.qrc
