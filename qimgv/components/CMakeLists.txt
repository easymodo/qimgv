## SOURCES FOR QIMGV COMPONENTS

target_sources(qimgv PRIVATE
    directorymodel.cpp
    directorypresenter.cpp

    scriptmanager/scriptmanager.cpp
    actionmanager/actionmanager.cpp

    cache/cache.cpp
    cache/cacheitem.cpp
    cache/thumbnailcache.cpp

    loader/loader.cpp
    loader/loaderrunnable.cpp

    scaler/scaler.cpp
    scaler/scalerrunnable.cpp

    thumbnailer/thumbnailer.cpp
    thumbnailer/thumbnailerrunnable.cpp

    directorymanager/directorymanager.cpp

    directorymanager/watchers/directorywatcher.cpp
    directorymanager/watchers/dummywatcher.cpp
    directorymanager/watchers/watcherevent.cpp
    directorymanager/watchers/watcherworker.cpp

)

if(APPLE)
    #target_sources(qimgv PRIVATE
    #    directorymanager/watchers/linux/linuxfsevent.cpp
    #    directorymanager/watchers/linux/linuxwatcher.cpp
    #    directorymanager/watchers/linux/linuxworker.cpp)
elseif(UNIX)
    target_sources(qimgv PRIVATE
        directorymanager/watchers/linux/linuxfsevent.cpp
        directorymanager/watchers/linux/linuxwatcher.cpp
        directorymanager/watchers/linux/linuxworker.cpp)
elseif(WIN32)
    target_sources(qimgv PRIVATE
        directorymanager/watchers/windows/windowswatcher.cpp
        directorymanager/watchers/windows/windowsworker.cpp)
endif()
