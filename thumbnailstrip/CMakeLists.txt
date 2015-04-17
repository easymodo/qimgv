include_directories(${CMAKE_SOURCE_DIR})
set (SOURCES customscene.cpp thumbnailstrip.cpp)
add_library(thumbnailstrip SHARED ${SOURCES})
target_link_libraries(thumbnailstrip Qt5::Widgets Qt5::Concurrent)
