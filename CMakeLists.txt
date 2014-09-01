cmake_minimum_required(VERSION 2.8.8)
project(qimgv)

set (CMAKE_AUTOMOC ON)
set (CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Widgets REQUIRED)

set (qimgv_SOURCES
        main.cpp
        mainwindow.cpp
        fileinfo.cpp
        controlsoverlay.cpp
        mapoverlay.cpp
        infooverlay.cpp
        core.cpp
        directorymanager.cpp
        image.cpp
        imagecache.cpp
        imageviewer.cpp
        imageloader.cpp
        opendialog.cpp
    )

set (qimgv_RESOURCES
        resources.qrc
    )

QT5_ADD_RESOURCES(qimgv_RESOURCES_RCC ${qimgv_RESOURCES})

add_executable (qimgv 
    ${qimgv_SOURCES}
    ${qimgv_RESOURCES_RCC}
)

qt5_use_modules (qimgv Widgets)
