cmake_minimum_required(VERSION 3.0)
project(qimgv)

set(CMAKE_BUILD_TYPE "Release")

# COMPILE FLAGS
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3")

# AUTOMOC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)


# USER OPTIONS
# Usage: cmake -DVIDEO_SUPPORT=ON [...]
option(VIDEO_SUPPORT "Enable video support" OFF)
option(KDE_SUPPORT "Support blur when using KDE" OFF)


# FIND PACKAGES
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake" ${CMAKE_MODULE_PATH})
find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Concurrent REQUIRED)
if(KDE_SUPPORT) # do i also need one for mpv?
    find_package(KF5WindowSystem REQUIRED)
endif()

# INCLUDE SOURCES
file(GLOB SRC *.cpp)
list(REMOVE_ITEM SRC moc_*.cpp)
list(REMOVE_ITEM SRC *_automoc.cpp)
include(CMake/SRC_Components.cmake)
include(CMake/SRC_SourceContainers.cmake)
include(CMake/SRC_Utils.cmake)
include(CMake/SRC_Gui.cmake)


# ADD RESOURCES
QT5_ADD_RESOURCES(RES resources.qrc)


# ADD EXECUTABLE
add_executable(qimgv ${SRC} ${SRC_Components} ${SRC_SourceContainers} ${SRC_Utils} ${SRC_Gui} ${RES})


# LINK STUFF
target_link_libraries(qimgv Qt5::Core Qt5::Widgets Qt5::Concurrent)

# OPTION DEFINITIONS, LINKING
if(KDE_SUPPORT)
    target_link_libraries(qimgv KF5::WindowSystem)
    add_definitions(-DUSE_KDE_BLUR)
endif()
if(VIDEO_SUPPORT)
    target_link_libraries(qimgv mpv)
    add_definitions(-DUSE_MPV)
endif()


## INSTALLATION

# binary
install(PROGRAMS ${CMAKE_BINARY_DIR}/qimgv
	DESTINATION bin)
# .desktop
install(FILES distrib/qimgv.desktop
	DESTINATION share/applications)
# icons
install(FILES distrib/hicolor/16x16/apps/qimgv.png
	DESTINATION share/icons/hicolor/16x16/apps)
install(FILES distrib/hicolor/22x22/apps/qimgv.png
	DESTINATION share/icons/hicolor/22x22/apps)
install(FILES distrib/hicolor/24x24/apps/qimgv.png
	DESTINATION share/icons/hicolor/24x24/apps)
install(FILES distrib/hicolor/32x32/apps/qimgv.png
	DESTINATION share/icons/hicolor/32x32/apps)
install(FILES distrib/hicolor/36x36/apps/qimgv.png
	DESTINATION share/icons/hicolor/36x36/apps)
install(FILES distrib/hicolor/48x48/apps/qimgv.png
	DESTINATION share/icons/hicolor/48x48/apps)
install(FILES distrib/hicolor/64x64/apps/qimgv.png
	DESTINATION share/icons/hicolor/64x64/apps)
install(FILES distrib/hicolor/128x128/apps/qimgv.png
	DESTINATION share/icons/hicolor/128x128/apps)
install(FILES distrib/hicolor/256x256/apps/qimgv.png
	DESTINATION share/icons/hicolor/256x256/apps)
install(FILES distrib/hicolor/scalable/apps/qimgv.svg
	DESTINATION share/icons/hicolor/scalable/apps)
