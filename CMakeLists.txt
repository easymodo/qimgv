cmake_minimum_required(VERSION 2.8)

project(qimgv)
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Concurrent REQUIRED)
#include_directories(${Qt5Widgets_INCLUDES})
#include_directories(${Qt5Concurrent_INCLUDES})
#add_definitions(${Qt5Widgets_DEFINITIONS})
#add_definitions(${Qt5Concurrent_DEFINITIONS})
#set(CMAKE_CXX_FLAGS "${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")
#set(CMAKE_CXX_FLAGS "${Qt5Concurrent_EXECUTABLE_COMPILE_FLAGS}")

#add_subdirectory(src)
#add_subdirectory(res)

FILE (GLOB SRC *.cpp)

QT5_WRAP_UI (GUI settingsdialog.ui)
QT5_ADD_RESOURCES(RES resources.qrc)

add_executable(qimgv ${SRC} ${RES} ${GUI}) 

#target_link_libraries(qimgv ${Qt5Widgets_LIBRARIES})
#target_link_libraries(qimgv ${Qt5Concurrent_LIBRARIES})

qt5_use_modules (qimgv Core Widgets Concurrent)

#install(TARGETS qimgv DESTINATION bin)
