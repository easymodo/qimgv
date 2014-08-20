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
    fileinfo.cpp \
    controlsoverlay.cpp

HEADERS  += mainwindow.h \
    zzscrollarea.h \
    infooverlay.h \
    fileinfo.h \
    controlsoverlay.h

FORMS    +=

RESOURCES += \
    resources.qrc
