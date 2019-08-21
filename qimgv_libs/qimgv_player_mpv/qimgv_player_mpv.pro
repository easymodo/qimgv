#-------------------------------------------------
#
# Project created by QtCreator 2019-08-21T00:24:16
#
#-------------------------------------------------

QT       += widgets

TARGET = qimgv_player_mpv
TEMPLATE = lib

DEFINES += QIMGV_PLAYER_MPV_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

unix {
    QT_CONFIG -= no-pkg-config
    CONFIG += link_pkgconfig
    PKGCONFIG += mpv
}
windows {
    LIBS += -L$$PWD/mpv-dev/lib/ -llibmpv
    INCLUDEPATH += $$PWD/mpv-dev/include
    DEPENDPATH += $$PWD/mpv-dev
}

SOURCES += \
        src/mpvwidget.cpp \
        src/videoplayer.cpp \
        src/videoplayermpv.cpp

HEADERS += \
        src/mpvwidget.h \
        src/videoplayer.h \
        src/videoplayermpv.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
