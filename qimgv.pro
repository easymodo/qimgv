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

win32:LIBS += -L$$PWD/mpv-dev/lib/ -llibmpv
win32:INCLUDEPATH += $$PWD/mpv-dev/include
win32:DEPENDPATH += $$PWD/mpv-dev

SOURCES += \
    main.cpp \
    core.cpp \
    components/actionmanager/actionmanager.cpp \
    components/cache/cache.cpp \
    components/cache/thumbnailcache.cpp \
    components/directorymanager/directorymanager.cpp \
    components/directorymanager/watcherwindows.cpp \
    components/loader/loader.cpp \
    components/loader/loaderrunnable.cpp \
    components/scaler/scaler.cpp \
    components/scaler/scalerrunnable.cpp \
    components/thumbnailer/thumbnailer.cpp \
    gui/mainwindow.cpp \
    gui/opendialog.cpp \
    gui/settingsdialog.cpp \
    gui/resizedialog.cpp \
    gui/customwidgets/clickablelabel.cpp \
    gui/customwidgets/clickablewidget.cpp \
    gui/customwidgets/iconbutton.cpp \
    gui/customwidgets/overlaywidget.cpp \
    gui/customwidgets/settingsshortcutwidget.cpp \
    gui/customwidgets/slidehpanel.cpp \
    gui/customwidgets/slidepanel.cpp \
    gui/customwidgets/slidevpanel.cpp \
    gui/overlays/controlsoverlay.cpp \
    gui/overlays/cropoverlay.cpp \
    gui/overlays/floatingmessage.cpp \
    gui/overlays/infooverlay.cpp \
    gui/overlays/mapoverlay.cpp \
    gui/panels/mainpanel/mainpanel.cpp \
    gui/panels/mainpanel/thumbnaillabel.cpp \
    gui/panels/mainpanel/thumbnailstrip.cpp \
    gui/panels/mainpanel/thumbnailview.cpp \
    gui/panels/sidepanel/toolbox.cpp \
    gui/viewers/imageviewer.cpp \
    gui/viewers/mpvwidget.cpp \
    gui/viewers/videoplayergl.cpp \
    gui/viewers/viewerwidget.cpp \
    sourcecontainers/clip.cpp \
    sourcecontainers/image.cpp \
    sourcecontainers/imageanimated.cpp \
    sourcecontainers/imageinfo.cpp \
    sourcecontainers/imagestatic.cpp \
    sourcecontainers/thumbnail.cpp \
    sourcecontainers/video.cpp \
    utils/imagefactory.cpp \
    utils/imagelib.cpp \
    utils/sleep.cpp \
    utils/stuff.cpp \
    utils/wallpapersetter.cpp \
    settings.cpp \
    gui/copydialog.cpp \
    gui/customwidgets/pathselectorwidget.cpp \
    components/cache/cacheitem.cpp \
    components/thumbnailer/thumbnailerrunnable.cpp

HEADERS += \
    core.h \
    components/actionmanager/actionmanager.h \
    components/cache/cache.h \
    components/cache/thumbnailcache.h \
    components/directorymanager/directorymanager.h \
    components/directorymanager/watcherwindows.h \
    components/loader/loader.h \
    components/loader/loaderrunnable.h \
    components/scaler/scaler.h \
    components/scaler/scalerrequest.h \
    components/scaler/scalerrunnable.h \
    components/thumbnailer/thumbnailer.h \
    gui/mainwindow.h \
    gui/opendialog.h \
    gui/settingsdialog.h \
    gui/resizedialog.h \
    gui/customwidgets/clickablelabel.h \
    gui/customwidgets/clickablewidget.h \
    gui/customwidgets/iconbutton.h \
    gui/customwidgets/overlaywidget.h \
    gui/customwidgets/settingsshortcutwidget.h \
    gui/customwidgets/slidehpanel.h \
    gui/customwidgets/slidepanel.h \
    gui/customwidgets/slidevpanel.h \
    gui/overlays/controlsoverlay.h \
    gui/overlays/cropoverlay.h \
    gui/overlays/floatingmessage.h \
    gui/overlays/infooverlay.h \
    gui/overlays/mapoverlay.h \
    gui/panels/mainpanel/mainpanel.h \
    gui/panels/mainpanel/thumbnaillabel.h \
    gui/panels/mainpanel/thumbnailstrip.h \
    gui/panels/mainpanel/thumbnailview.h \
    gui/panels/sidepanel/toolbox.h \
    gui/viewers/imageviewer.h \
    gui/viewers/mpvwidget.h \
    gui/viewers/videoplayergl.h \
    gui/viewers/viewerwidget.h \
    sourcecontainers/clip.h \
    sourcecontainers/image.h \
    sourcecontainers/imageanimated.h \
    sourcecontainers/imageinfo.h \
    sourcecontainers/imagestatic.h \
    sourcecontainers/thumbnail.h \
    sourcecontainers/video.h \
    utils/imagefactory.h \
    utils/imagelib.h \
    utils/stuff.h \
    utils/wallpapersetter.h \
    settings.h \
    gui/copydialog.h \
    gui/customwidgets/pathselectorwidget.h \
    components/cache/cacheitem.h \
    components/thumbnailer/thumbnailerrunnable.h

FORMS += \
    gui/settingsdialog.ui

RESOURCES += \
    resources.qrc
