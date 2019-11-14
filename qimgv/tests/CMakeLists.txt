enable_testing()
find_package(Qt5 REQUIRED COMPONENTS Test Widgets)

include_directories(${CMAKE_SOURCE_DIR})

add_executable(unit_tests test_mapoverlay.cpp)
target_link_libraries(unit_tests PRIVATE Qt5::Test Qt5::Widgets)

add_test(NAME QUI_TEST COMMAND unit_tests)
