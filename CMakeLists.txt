cmake_minimum_required(VERSION 2.8)

project(qimgv)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Concurrent REQUIRED)
find_package(Qt5Multimedia REQUIRED)
find_package(Qt5MultimediaWidgets REQUIRED)

include_directories(res ${Qt5Widgets_INCLUDES} ${Qt5Concurrent_INCLUDES} ${Qt5Multimedia_INCLUDES} ${Qt5MultimediaWidgets_INCLUDES})

add_definitions(${Qt5Widgets_DEFINITIONS} ${Qt5Concurrent_DEFINITIONS} ${Qt5Multimedia_DEFINITIONS} ${Qt5MultimediaWidgets_DEFINITIONS})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS} ${Qt5Concurrent_EXECUTABLE_COMPILE_FLAGS} ${Qt5Multimedia_EXECUTABLE_COMPILE_FLAGS} ${Qt5MultimediaWidgets_EXECUTABLE_COMPILE_FLAGS} -std=c++11")

#add_subdirectory(tests)
add_subdirectory(overlays)
add_subdirectory(lib)
add_subdirectory(thumbnailPanel)
add_subdirectory(sourceContainers)
add_subdirectory(viewers)
add_subdirectory(customWidgets)
add_subdirectory(filesystemWatchers)

FILE (GLOB SRC *.cpp)
list (REMOVE_ITEM SRC moc_*.cpp)
list (REMOVE_ITEM SRC *_automoc.cpp)

QT5_WRAP_UI (GUI settingsdialog.ui)

QT5_ADD_RESOURCES(RES resources.qrc)

add_executable(qimgv ${SRC} ${RES} ${GUI})

target_link_libraries(qimgv Qt5::Widgets Qt5::Concurrent overlays imagelib sourcecontainers viewers customwidgets thumbnailpanel filesystemWatchers)
     
qt5_use_modules (qimgv Core Widgets Concurrent)

# binary
install(PROGRAMS qimgv DESTINATION bin/)
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

