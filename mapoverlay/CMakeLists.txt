include_directories(${CMAKE_SOURCE_DIR})
set (SOURCES mapoverlay.cpp)
add_library(mapoverlay SHARED ${SOURCES})
target_link_libraries(mapoverlay Qt5::Widgets)