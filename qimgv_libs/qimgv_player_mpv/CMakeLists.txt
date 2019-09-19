cmake_minimum_required(VERSION 3.9)
project(qimgv_player_mpv VERSION 1.0 DESCRIPTION "video player widget for qimgv (using mpv)")

# COMPILE FLAGS
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3")
set(CMAKE_AUTOMOC ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)

include(GNUInstallDirs)

if(WIN32)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/mpv-dev/include)
    if("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
        link_directories(${CMAKE_CURRENT_SOURCE_DIR}/mpv-dev/x86_64)
    else()
        link_directories(${CMAKE_CURRENT_SOURCE_DIR}/mpv-dev/x86)
    endif()
endif()

add_library(qimgv_player_mpv SHARED
    src/videoplayer.cpp
    src/mpvwidget.cpp
    src/videoplayermpv.cpp)
    
target_link_libraries(qimgv_player_mpv Qt5::Core Qt5::Widgets mpv)

set_target_properties(qimgv_player_mpv PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1)

target_include_directories(qimgv_player_mpv PRIVATE src)

add_definitions(-DQIMGV_PLAYER_MPV_LIBRARY)

install(TARGETS qimgv_player_mpv LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
