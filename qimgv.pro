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
    zzscrollarea.cpp \
    infooverlay.cpp \
    controlsoverlay.cpp \
    mapoverlay.cpp \
    zzlabel.cpp \
    directorymanager.cpp \
    controller.cpp \
    imagepreloader.cpp

HEADERS  += mainwindow.h \
    zzscrollarea.h \
    infooverlay.h \
    controlsoverlay.h \
    mapoverlay.h \
    zzlabel.h \
    directorymanager.h \
    controller.h \
    ImageContainer.h \
    imagepreloader.h

FORMS    +=

RESOURCES += \
    resources.qrc
