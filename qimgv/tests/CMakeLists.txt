enable_testing()
find_package(Qt5 COMPONENTS Test)

include_directories(${CMAKE_SOURCE_DIR})

set(TEST_SOURCES test_mapoverlay.cpp)

add_executable(unit_tests ${TEST_SOURCES})
target_link_libraries(unit_tests
    Qt5::Widgets
    Qt5::Test
    mapoverlay
)

add_test(NAME QUI_TEST COMMAND unit_tests)
