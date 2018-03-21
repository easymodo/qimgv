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

SOURCES += \
    main.cpp \
    core.cpp \
    components/actionmanager/actionmanager.cpp \
    components/cache/cache.cpp \
    components/cache/thumbnailcache.cpp \
    components/directorymanager/directorymanager.cpp \
    components/directorymanager/watchers/directorywatcher.cpp \
    components/loader/loader.cpp \
    components/loader/loaderrunnable.cpp \
    components/scaler/scaler.cpp \
    components/scaler/scalerrunnable.cpp \
    components/thumbnailer/thumbnailer.cpp \
    gui/mainwindow.cpp \
    gui/dialogs/settingsdialog.cpp \
    gui/dialogs/resizedialog.cpp \
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
    gui/viewers/imageviewer.cpp \
    gui/viewers/mpvwidget.cpp \
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
    gui/overlays/copyoverlay.cpp \
    gui/customwidgets/pathselectorwidget.cpp \
    components/cache/cacheitem.cpp \
    components/thumbnailer/thumbnailerrunnable.cpp \
    components/directorymanager/watchers/watcherevent.cpp \
    components/directorymanager/watchers/dummywatcher.cpp \
    components/directorymanager/watchers/watcherworker.cpp \
    sharedresources.cpp \
    gui/panels/croppanel/croppanel.cpp \
    gui/panels/sidepanel/sidepanel.cpp \
    gui/customwidgets/spinboxinputfix.cpp \
    gui/customwidgets/sidepanelwidget.cpp \
    gui/viewers/videoplayer.cpp \
    gui/viewers/videoplayermpv.cpp \
    gui/viewers/videoplayermpvproxy.cpp \
    gui/overlays/saveconfirmoverlay.cpp \
    gui/customwidgets/floatingwidget.cpp \
    gui/customwidgets/containerwidget.cpp \
    appversion.cpp \
    gui/overlays/changelogwindow.cpp


HEADERS += \
    core.h \
    components/actionmanager/actionmanager.h \
    components/cache/cache.h \
    components/cache/thumbnailcache.h \
    components/directorymanager/directorymanager.h \
    components/directorymanager/watchers/directorywatcher_p.h \
    components/directorymanager/watchers/directorywatcher.h \
    components/directorymanager/watchers/watcherevent.h \
    components/loader/loader.h \
    components/loader/loaderrunnable.h \
    components/scaler/scaler.h \
    components/scaler/scalerrequest.h \
    components/scaler/scalerrunnable.h \
    components/thumbnailer/thumbnailer.h \
    gui/mainwindow.h \
    gui/dialogs/settingsdialog.h \
    gui/dialogs/resizedialog.h \
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
    gui/viewers/imageviewer.h \
    gui/viewers/mpvwidget.h \
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
    gui/overlays/copyoverlay.h \
    gui/customwidgets/pathselectorwidget.h \
    components/cache/cacheitem.h \
    components/thumbnailer/thumbnailerrunnable.h \
    components/directorymanager/watchers/dummywatcher.h \
    components/directorymanager/watchers/watcherworker.h \
    sharedresources.h \
    gui/panels/croppanel/croppanel.h \
    gui/panels/sidepanel/sidepanel.h \
    gui/customwidgets/spinboxinputfix.h \
    gui/customwidgets/sidepanelwidget.h \
    gui/viewers/videoplayer.h \
    gui/viewers/videoplayermpv.h \
    gui/viewers/videoplayermpvproxy.h \
    gui/overlays/saveconfirmoverlay.h \
    gui/customwidgets/floatingwidget.h \
    gui/customwidgets/containerwidget.h \
    appversion.h \
    gui/overlays/changelogwindow.h

FORMS += \
    gui/dialogs/settingsdialog.ui \
    gui/dialogs/resizedialog.ui \
    gui/panels/croppanel/croppanel.ui \
    gui/panels/sidepanel/sidepanel.ui \
    gui/overlays/copyoverlay.ui \
    gui/overlays/saveconfirmoverlay.ui \
    gui/overlays/floatingmessage.ui \
    gui/customwidgets/pathselectorwidget.ui \
    gui/overlays/changelogwindow.ui

RESOURCES += \
    resources.qrc

unix {
    QT_CONFIG -= no-pkg-config
    CONFIG += link_pkgconfig
    PKGCONFIG += mpv

    SOURCES += \
        components/directorymanager/watchers/linux/linuxworker.cpp \
        components/directorymanager/watchers/linux/linuxwatcher.cpp \
        components/directorymanager/watchers/linux/linuxfsevent.cpp

    HEADERS += \
        components/directorymanager/watchers/linux/linuxfsevent.h \
        components/directorymanager/watchers/linux/linuxwatcher.h \
        components/directorymanager/watchers/linux/linuxworker.h \
        components/directorymanager/watchers/linux/linuxwatcher_p.h
}

windows {
#    SOURCES += \
#        components/directorymanager/watchers/windows/windowsdirectorywatcher.cpp \
#        components/directorymanager/watchers/windows/windowswatcherworker.cpp

#    HEADERS += \
#        components/directorymanager/watchers/windows/windowsdirectorywatcher_p.h \
#        components/directorymanager/watchers/windows/windowsdirectorywatcher.h \
#        components/directorymanager/watchers/windows/windowswatcherbackgroundworker.h
}

win32:LIBS += -L$$PWD/mpv-dev/lib/ -llibmpv
win32:INCLUDEPATH += $$PWD/mpv-dev/include
win32:DEPENDPATH += $$PWD/mpv-dev

DISTFILES += \
    README.md
