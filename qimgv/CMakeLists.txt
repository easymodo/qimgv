# INCLUDE SOURCES
file(GLOB SRC *.cpp)
list(REMOVE_ITEM SRC moc_*.cpp)
list(REMOVE_ITEM SRC *_automoc.cpp)
include(CMake/SRC_Components.cmake)
include(CMake/SRC_SourceContainers.cmake)
include(CMake/SRC_Utils.cmake)
include(CMake/SRC_Gui.cmake)
include(CMake/SRC_UI_Files.cmake)

# ADD RESOURCES
QT5_ADD_RESOURCES(RES resources.qrc)

# ADD EXECUTABLE
add_executable(qimgv ${SRC} ${SRC_Components} ${SRC_SourceContainers} ${SRC_Utils} ${SRC_Gui} ${RES} ${SRC_UI_Files})

# LINK STUFF
target_link_libraries(qimgv Qt5::Core Qt5::Widgets Qt5::Concurrent stdc++fs)
#target_link_libraries(qimgv Qt5::Core Qt5::Widgets Qt5::Concurrent Qt5::QWindowsIntegrationPlugin stdc++fs)

# OPTION DEFINITIONS, LINKING
if(EXIV2)
    target_link_libraries(qimgv exiv2)
    add_definitions(-DUSE_EXIV2)
endif()
if(KDE_SUPPORT)
    target_link_libraries(qimgv KF5::WindowSystem)
    add_definitions(-DUSE_KDE_BLUR)
endif()
if(VIDEO_SUPPORT)
    add_definitions(-DUSE_MPV)
endif()

## INSTALLATION

install(PROGRAMS ${CMAKE_BINARY_DIR}/qimgv/qimgv
   DESTINATION bin)
   
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
install(FILES distrib/hicolor/36x36/apps/qimgv.png
	DESTINATION share/icons/hicolor/36x36/apps)
install(FILES distrib/hicolor/48x48/apps/qimgv.png
	DESTINATION share/icons/hicolor/48x48/apps)
install(FILES distrib/hicolor/64x64/apps/qimgv.png
	DESTINATION share/icons/hicolor/64x64/apps)
install(FILES distrib/hicolor/128x128/apps/qimgv.png
	DESTINATION share/icons/hicolor/128x128/apps)
install(FILES distrib/hicolor/256x256/apps/qimgv.png
	DESTINATION share/icons/hicolor/256x256/apps)
install(FILES distrib/hicolor/scalable/apps/qimgv.svg
	DESTINATION share/icons/hicolor/scalable/apps)
