# qimgv root
cmake_minimum_required(VERSION 3.13)
project(qimgv
    VERSION 1.0.4
    HOMEPAGE_URL "https://github.com/easymodo/qimgv"
    LANGUAGES CXX)

message(STATUS "Build configuration: " ${CMAKE_BUILD_TYPE})

# check for gcc
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND
        CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
    message(FATAL_ERROR "\n!!! THIS PROJECT REQUIRES GCC 9.0 OR LATER !!!\n")
endif()

# AUTOMOC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# USER OPTIONS
# Usage: cmake -DVIDEO_SUPPORT=ON [...]
option(EXIV2 "For reading/writing exif tags" ON)
option(VIDEO_SUPPORT "Enable video support" ON)
option(OPENCV_SUPPORT "Enable HQ scaling via OpenCV" ON)
option(KDE_SUPPORT "Support blur when using KDE" OFF)
option(USE_QT5 "Build with Qt5 if available" ON)

if(UNIX AND NOT APPLE)
    set(QT_EXTERN_PATH "" CACHE STRING "Tell compile external QT path, example: (/opt/Qt/6.2.0/gcc_64)")
    string(COMPARE EQUAL "${QT_EXTERN_PATH}" "" result)
    if (NOT result)
        set(CMAKE_INSTALL_RPATH "${QT_EXTERN_PATH}/lib")
    endif()
endif()

# FIND PACKAGES
if(USE_QT5) # Try Qt5 first if USE_QT5 is set
    find_package(Qt5 COMPONENTS Widgets QUIET)
    if(Qt5_FOUND)
        find_package(QT NAMES Qt5 REQUIRED COMPONENTS Core Widgets Svg PrintSupport)
    else()
        find_package(QT NAMES Qt6 REQUIRED COMPONENTS Core Widgets Svg PrintSupport OpenGLWidgets)
    endif()
else() # Try Qt6 otherwise
    find_package(Qt6 COMPONENTS Widgets QUIET)
    if(Qt6_FOUND)
        find_package(QT NAMES Qt6 REQUIRED COMPONENTS Core Widgets Svg PrintSupport OpenGLWidgets)
    else()
        find_package(QT NAMES Qt5 REQUIRED COMPONENTS Core Widgets Svg PrintSupport)
    endif()
endif()

message(STATUS "Qt Version: " ${QT_VERSION})
if(QT_VERSION_MAJOR GREATER_EQUAL 6)
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets Svg PrintSupport OpenGLWidgets)
else()
    find_package(Qt${QT_VERSION_MAJOR} 5.12 REQUIRED COMPONENTS Core Widgets Svg PrintSupport)
endif()

if(EXIV2)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(Exiv2 REQUIRED IMPORTED_TARGET exiv2)
endif()

if(KDE_SUPPORT)
    # Try KF6 first, fallback to KF5
    find_package(KF6WindowSystem QUIET)
    if(KF6WindowSystem_FOUND)
        message(STATUS "Using KF6WindowSystem")
    else()
        find_package(KF5WindowSystem REQUIRED)
        message(STATUS "Using KF5WindowSystem")
    endif()
endif()

if(OPENCV_SUPPORT)
    find_package(PkgConfig REQUIRED)
    find_package(OpenCV REQUIRED core imgproc)
endif()

##############################################################

add_subdirectory(qimgv)

if(VIDEO_SUPPORT)
    add_subdirectory(plugins/player_mpv)
endif()

##############################################################
# CPack configuration for Debian package generation
##############################################################

set(CPACK_PACKAGE_NAME "qimgv")
set(CPACK_PACKAGE_VENDOR "easymodo")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Fast and configurable image viewer")
set(CPACK_PACKAGE_DESCRIPTION "qimgv is a fast and easy to use image viewer with EXIF sorting support")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_CONTACT "https://github.com/easymodo/qimgv")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")

# Debian specific
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "qimgv developers")
set(CPACK_DEBIAN_PACKAGE_SECTION "graphics")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)

# Auto-detect dependencies
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)

# Generator
set(CPACK_GENERATOR "DEB")

include(CPack)
