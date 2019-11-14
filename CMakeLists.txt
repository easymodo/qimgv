# qimgv root
cmake_minimum_required(VERSION 3.9)

project(qimgv)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()
message(STATUS "Build configuration: " ${CMAKE_BUILD_TYPE})

# detect if libstdc++ is in use to know if libstdc++fs should be linked
include(CheckCXXSourceCompiles)
check_cxx_source_compiles("#include <iostream>
    #ifndef __GLIBCXX__
    #error not using libstdc+++
    #endif
    int main() { return 0; }"
    USING_LIBSTDCXX)

# check requirements for std::filesystem
if (CMAKE_COMPILER_IS_GNUCC AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 8.0)
    message(FATAL_ERROR "\n!!! THIS PROJECT REQUIRES GCC 8 OR LATER !!!\n"
                    "If you already have gcc 8+ you can force it like this:\n"
                    "CC=gcc-8 CXX=g++-8 cmake [...etc]")
    # force gcc 8
    #set(CMAKE_C_COMPILER "gcc-8")
    #set(CMAKE_CXX_COMPILER "g++-8")
endif()

# AUTOMOC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# USER OPTIONS
# Usage: cmake -DVIDEO_SUPPORT=ON [...]
option(EXIV2 "For reading/writing exif tags" ON)
option(VIDEO_SUPPORT "Enable video support" ON)
option(KDE_SUPPORT "Support blur when using KDE" OFF)

# FIND PACKAGES
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake" ${CMAKE_MODULE_PATH})
find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Concurrent REQUIRED)

if(EXIV2)
    find_package(Exiv2 REQUIRED)
    if(NOT ${EXIV2_FOUND})
        message(FATAL_ERROR "** Unable to locate Exiv2... is it installed?")
    endif()
endif()

if(KDE_SUPPORT) # do i also need one for mpv?
    find_package(KF5WindowSystem REQUIRED)
endif()

# generate proper GUI program on specified platform
if(WIN32) # Check if we are on Windows
        if(MSVC) # Check if we are using the Visual Studio compiler
                set_target_properties(${PROJECT_NAME} PROPERTIES
                        WIN32_EXECUTABLE YES
                        LINK_FLAGS "/ENTRY:mainCRTStartup"
                )
        elseif(CMAKE_COMPILER_IS_GNUCXX)
                SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mwindows") # Not tested
        else()
                message(SEND_ERROR "You are using an unsupported Windows compiler! (Not MSVC or GCC)")
        endif(MSVC)
elseif(APPLE)
        set_target_properties(${PROJECT_NAME} PROPERTIES
                        MACOSX_BUNDLE YES
        )
elseif(UNIX)
        # Nothing special required
else()
        message(SEND_ERROR "You are on an unsupported platform! (Not Win32, Mac OS X or Unix)")
endif(WIN32)

##############################################################

add_subdirectory(qimgv)

if(VIDEO_SUPPORT)
    add_subdirectory(qimgv_libs/qimgv_player_mpv)
endif()
