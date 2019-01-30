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

#CONFIG += WITH_MPV
CONFIG += WITH_KDE_BLUR
CONFIG += PORTABLE

# video support
WITH_MPV {
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
    DEFINES += USE_MPV
    SOURCES += gui/viewers/playermpv/mpvwidget.cpp
    SOURCES += gui/viewers/playermpv/videoplayermpv.cpp
    HEADERS += gui/viewers/playermpv/mpvwidget.h
    HEADERS += gui/viewers/playermpv/videoplayermpv.h
} else {
    SOURCES += gui/viewers/playerdummy/videoplayerdummy.cpp
    HEADERS += gui/viewers/playerdummy/videoplayerdummy.h
}

unix {
    WITH_KDE_BLUR {
        QT += KWindowSystem
        DEFINES += USE_KDE_BLUR
    }
}

PORTABLE {
    windows {
        DEFINES += IS_PORTABLE
    }
}

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
    gui/customwidgets/overlaywidget.cpp \
    gui/customwidgets/slidehpanel.cpp \
    gui/customwidgets/slidepanel.cpp \
    gui/customwidgets/slidevpanel.cpp \
    gui/overlays/controlsoverlay.cpp \
    gui/overlays/cropoverlay.cpp \
    gui/overlays/floatingmessage.cpp \
    gui/overlays/infooverlay.cpp \
    gui/overlays/mapoverlay.cpp \
    gui/panels/mainpanel/mainpanel.cpp \
    gui/panels/mainpanel/thumbnailstrip.cpp \
    gui/viewers/imageviewer.cpp \
    gui/viewers/viewerwidget.cpp \
    sourcecontainers/clip.cpp \
    sourcecontainers/image.cpp \
    sourcecontainers/imageanimated.cpp \
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
    gui/overlays/saveconfirmoverlay.cpp \
    gui/customwidgets/floatingwidget.cpp \
    appversion.cpp \
    gui/overlays/changelogwindow.cpp \
    components/scriptmanager/scriptmanager.cpp \
    gui/viewers/videoplayerinitproxy.cpp \
    gui/dialogs/shortcutcreatordialog.cpp \
    gui/customwidgets/keysequenceedit.cpp \
    utils/inputmap.cpp \
    shortcutbuilder.cpp \
    utils/actions.cpp \
    utils/script.cpp \
    gui/dialogs/scripteditordialog.cpp \
    gui/customwidgets/overlaycontainerwidget.cpp \
    gui/viewers/documentwidget.cpp \
    sourcecontainers/documentinfo.cpp \
    gui/overlays/videocontrols.cpp \
    gui/customwidgets/videoslider.cpp \
    gui/flowlayout.cpp \
    gui/customwidgets/thumbnailview.cpp \
    gui/customwidgets/thumbnailwidget.cpp \
    gui/folderview/folderview.cpp \
    gui/folderview/foldergridview.cpp \
    gui/folderview/thumbnailgridwidget.cpp \
    gui/centralwidget.cpp \
    gui/contextmenu.cpp \
    gui/customwidgets/contextmenuitem.cpp \
    utils/helprunner.cpp \
    gui/customwidgets/actionbutton.cpp

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
    gui/customwidgets/overlaywidget.h \
    gui/customwidgets/slidehpanel.h \
    gui/customwidgets/slidepanel.h \
    gui/customwidgets/slidevpanel.h \
    gui/overlays/controlsoverlay.h \
    gui/overlays/cropoverlay.h \
    gui/overlays/floatingmessage.h \
    gui/overlays/infooverlay.h \
    gui/overlays/mapoverlay.h \
    gui/panels/mainpanel/mainpanel.h \
    gui/panels/mainpanel/thumbnailstrip.h \
    gui/viewers/imageviewer.h \
    gui/viewers/viewerwidget.h \
    sourcecontainers/clip.h \
    sourcecontainers/image.h \
    sourcecontainers/imageanimated.h \
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
    gui/overlays/saveconfirmoverlay.h \
    gui/customwidgets/floatingwidget.h \
    appversion.h \
    gui/overlays/changelogwindow.h \
    components/scriptmanager/scriptmanager.h \
    gui/viewers/videoplayerinitproxy.h \
    gui/dialogs/shortcutcreatordialog.h \
    gui/customwidgets/keysequenceedit.h \
    utils/inputmap.h \
    shortcutbuilder.h \
    utils/actions.h \
    utils/script.h \
    gui/dialogs/scripteditordialog.h \
    gui/customwidgets/overlaycontainerwidget.h \
    gui/viewers/documentwidget.h \
    sourcecontainers/documentinfo.h \
    gui/overlays/videocontrols.h \
    gui/customwidgets/videoslider.h \
    gui/flowlayout.h \
    gui/customwidgets/thumbnailview.h \
    gui/customwidgets/thumbnailwidget.h \
    gui/folderview/folderview.h \
    gui/folderview/foldergridview.h \
    gui/folderview/thumbnailgridwidget.h \
    gui/centralwidget.h \
    gui/contextmenu.h \
    gui/customwidgets/contextmenuitem.h \
    utils/numeric.h \
    utils/helprunner.h \
    gui/customwidgets/actionbutton.h

FORMS += \
    gui/dialogs/settingsdialog.ui \
    gui/dialogs/resizedialog.ui \
    gui/panels/croppanel/croppanel.ui \
    gui/panels/sidepanel/sidepanel.ui \
    gui/overlays/copyoverlay.ui \
    gui/overlays/saveconfirmoverlay.ui \
    gui/overlays/floatingmessage.ui \
    gui/customwidgets/pathselectorwidget.ui \
    gui/overlays/changelogwindow.ui \
    gui/dialogs/shortcutcreatordialog.ui \
    gui/dialogs/scripteditordialog.ui \
    gui/overlays/videocontrols.ui \
    gui/folderview/folderview.ui \
    gui/contextmenu.ui

RESOURCES += \
    resources.qrc

unix {
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

DISTFILES += \
    README.md
