cmake_minimum_required(VERSION 2.8)

project(qimgv)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Concurrent REQUIRED)

include_directories(res ${Qt5Widgets_INCLUDES} ${Qt5Concurrent_INCLUDES})

add_definitions(${Qt5Widgets_DEFINITIONS} ${Qt5Concurrent_DEFINITIONS})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS} ${Qt5Concurrent_EXECUTABLE_COMPILE_FLAGS} -std=c++11")

FILE (GLOB SRC *.cpp)

QT5_WRAP_UI (GUI settingsdialog.ui)

QT5_ADD_RESOURCES(RES resources.qrc)

add_executable(qimgv ${SRC} ${RES} ${GUI})

target_link_libraries(qimgv Qt5::Widgets Qt5::Concurrent mapoverlay lib thumbnailstrip)
     
qt5_use_modules (qimgv Core Widgets Concurrent)

install(FILES qimgv DESTINATION /usr/bin/)

add_subdirectory(tests)
add_subdirectory(mapoverlay)
add_subdirectory(lib)
add_subdirectory(thumbnailstrip)

