cmake_minimum_required(VERSION 3.0)

project(qimgv)

set(CMAKE_BUILD_TYPE RELEASE)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

SET(BASEPATH "${CMAKE_SOURCE_DIR}")
INCLUDE_DIRECTORIES("${BASEPATH}")

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Concurrent REQUIRED)

include_directories(res ${Qt5Widgets_INCLUDES} ${Qt5Concurrent_INCLUDES})

add_definitions(${Qt5Widgets_DEFINITIONS} ${Qt5Concurrent_DEFINITIONS})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS} ${Qt5Concurrent_EXECUTABLE_COMPILE_FLAGS} -std=c++11")

add_subdirectory(components/actionmanager)
add_subdirectory(components/cache)
add_subdirectory(components/directorymanager)
add_subdirectory(components/loader)
add_subdirectory(components/scaler)
add_subdirectory(components/thumbnailer)
add_subdirectory(gui)
add_subdirectory(gui/customwidgets)
add_subdirectory(gui/overlays)
add_subdirectory(gui/panels/mainpanel)
add_subdirectory(gui/panels/sidepanel)
add_subdirectory(gui/viewers)
add_subdirectory(sourcecontainers)
add_subdirectory(utils)

FILE (GLOB SRC *.cpp)
list (REMOVE_ITEM SRC moc_*.cpp)
list (REMOVE_ITEM SRC *_automoc.cpp)

QT5_ADD_RESOURCES(RES resources.qrc)

add_executable(qimgv ${SRC} ${RES})

target_link_libraries(qimgv actionmanager cache directorymanager loader scaler thumbnailer gui customwidgets overlays mainpanel sidepanel viewers utils sourcecontainers mpv)
     
qt5_use_modules (qimgv Core Widgets Concurrent)

# binary
install(PROGRAMS build/qimgv DESTINATION bin/)
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
install(FILES distrib/hicolor/48x48/apps/qimgv.png
	DESTINATION share/icons/hicolor/48x48/apps)
install(FILES distrib/hicolor/64x64/apps/qimgv.png
	DESTINATION share/icons/hicolor/64x64/apps)
install(FILES distrib/hicolor/128x128/apps/qimgv.png
	DESTINATION share/icons/hicolor/128x128/apps)
install(FILES distrib/hicolor/256x256/apps/qimgv.png
	DESTINATION share/icons/hicolor/256x256/apps)

