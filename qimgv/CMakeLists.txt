# ADD EXECUTABLE
add_executable(qimgv
    appversion.cpp
    core.cpp
    main.cpp
    settings.cpp
    sharedresources.cpp
    shortcutbuilder.cpp
    resources.qrc
    qimgv.rc)

# ADD SOURCES
add_subdirectory(components)
add_subdirectory(sourcecontainers)
add_subdirectory(gui)
add_subdirectory(utils)

# C++ STANDARD
target_compile_features(qimgv PRIVATE cxx_std_17)
set_target_properties(qimgv PROPERTIES CXX_EXTENSIONS OFF)

# LINK STUFF
target_link_libraries(qimgv PRIVATE Qt5::Core Qt5::Widgets Qt5::Concurrent)
if(USING_LIBSTDCXX)
    target_link_libraries(qimgv PRIVATE stdc++fs)
endif()

# OPTION DEFINITIONS, LINKING
if(EXIV2)
    target_link_libraries(qimgv PRIVATE PkgConfig::Exiv2)
    target_compile_definitions(qimgv PRIVATE USE_EXIV2)
endif()
if(KDE_SUPPORT)
    target_link_libraries(qimgv PRIVATE KF5::WindowSystem)
    target_compile_definitions(qimgv PRIVATE USE_KDE_BLUR)
endif()
if(VIDEO_SUPPORT)
    target_compile_definitions(qimgv PRIVATE USE_MPV)
endif()

# generate proper GUI program on specified platform
if(WIN32) # Check if we are on Windows
    if(MSVC) # Check if we are using the Visual Studio compiler
        set_target_properties(qimgv PROPERTIES WIN32_EXECUTABLE ON)
        target_link_options(qimgv PRIVATE "/ENTRY:mainCRTStartup")
    elseif(CMAKE_COMPILER_ID STREQUAL "GNU")
        target_compile_options(qimgv PRIVATE "-mwindows") # Not tested
    else()
        message(SEND_ERROR "You are using an unsupported Windows compiler! (Not MSVC or GCC)")
    endif(MSVC)
elseif(APPLE)
    set_target_properties(qimgv PROPERTIES MACOSX_BUNDLE YES)
elseif(NOT UNIX)
    message(SEND_ERROR "You are on an unsupported platform! (Not Win32, Mac OS X or Unix)")
endif()

## INSTALLATION
install(TARGETS qimgv)

# .desktop
install(FILES distrib/qimgv.desktop
    DESTINATION ${CMAKE_INSTALL_FULL_DATAROOTDIR}/applications)

# icons
foreach(icon IN ITEMS 16x16 22x22 24x24 32x32 36x36 48x48 64x64 128x128 256x256)
    install(FILES distrib/hicolor/${icon}/apps/qimgv.png DESTINATION
        ${CMAKE_INSTALL_FULL_DATAROOTDIR}/icons/hicolor/${icon}/apps)
endforeach()
install(FILES distrib/hicolor/scalable/apps/qimgv.svg
    DESTINATION ${CMAKE_INSTALL_FULL_DATAROOTDIR}/icons/hicolor/scalable/apps)
