include_directories(${CMAKE_SOURCE_DIR})
set (SOURCES mapoverlay.cpp)
add_library(mapoverlay STATIC ${SOURCES})
target_link_libraries(mapoverlay Qt5::Widgets)
