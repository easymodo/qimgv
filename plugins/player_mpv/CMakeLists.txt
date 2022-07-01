cmake_minimum_required(VERSION 3.11)
project(player_mpv
    DESCRIPTION "video player widget for qimgv (using mpv)"
    LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)

# only export CreatePlayerWidget function
ADD_DEFINITIONS(-DQIMGV_PLAYER_MPV_LIBRARY)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets)

if(NOT WIN32)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(Mpv REQUIRED IMPORTED_TARGET mpv)
endif()

include(GNUInstallDirs)

add_library(player_mpv MODULE
    src/videoplayer.cpp
    src/mpvwidget.cpp
    src/videoplayermpv.cpp
    src/qthelper.hpp)

target_compile_features(player_mpv PRIVATE cxx_std_11)

if(WIN32)
    target_link_libraries(player_mpv PRIVATE
        Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Widgets Mpv)
else()
    target_link_libraries(player_mpv PRIVATE
        Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Widgets PkgConfig::Mpv)
endif()

set_target_properties(player_mpv PROPERTIES
    CXX_EXTENSIONS OFF
    PREFIX "")

target_include_directories(player_mpv PRIVATE src)

if(WIN32)
    target_include_directories(player_mpv PRIVATE
        ${MPV_DIR}/include)
    target_link_directories(player_mpv PRIVATE
        ${MPV_DIR}/lib/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x86_64,i686>)
endif()

if(WIN32)
    install(TARGETS player_mpv LIBRARY DESTINATION "${CMAKE_INSTALL_BINDIR}/plugins")
else()
    install(TARGETS player_mpv LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}/qimgv")
endif()
