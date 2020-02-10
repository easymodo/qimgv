cmake_minimum_required(VERSION 3.11)
project(qimgv_player_mpv VERSION 1.0
    DESCRIPTION "video player widget for qimgv (using mpv)"
    LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)

find_package(Qt5 REQUIRED COMPONENTS Core Widgets)

find_package(PkgConfig REQUIRED)
pkg_check_modules(Mpv REQUIRED IMPORTED_TARGET mpv)

include(GNUInstallDirs)

add_library(qimgv_player_mpv SHARED
    src/videoplayer.cpp
    src/mpvwidget.cpp
    src/videoplayermpv.cpp)

target_compile_features(qimgv_player_mpv PRIVATE cxx_std_11)

target_link_libraries(qimgv_player_mpv PRIVATE
    Qt5::Core Qt5::Widgets PkgConfig::Mpv)

set_target_properties(qimgv_player_mpv PROPERTIES
    CXX_EXTENSIONS OFF
    VERSION ${PROJECT_VERSION}
    SOVERSION 1)

target_include_directories(qimgv_player_mpv PRIVATE src)

if(WIN32)
    target_include_directories(qimgv_player_mpv PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/mpv-dev/include)
    target_link_directories(qimgv_player_mpv PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/mpv-dev/$<IF$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x86_64,i686>)
endif()

target_compile_definitions(qimgv_player_mpv PRIVATE QIMGV_PLAYER_MPV_LIBRARY)

install(TARGETS qimgv_player_mpv LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
