set(SOURCES imagelib.cpp)
add_library(lib SHARED ${SOURCES})
target_link_libraries(lib Qt5::Widgets)
