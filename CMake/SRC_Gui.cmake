## SOURCES FOR QIMGV GUI

set(dir "gui")
file(GLOB SRC_Gui ${SRC_Gui} ${dir}/*.cpp)

set(dir "gui/customwidgets")
file(GLOB SRC_Gui ${SRC_Gui} ${dir}/*.cpp)

set(dir "gui/dialogs")
file(GLOB SRC_Gui ${SRC_Gui} ${dir}/*.cpp)

set(dir "gui/overlays")
file(GLOB SRC_Gui ${SRC_Gui} ${dir}/*.cpp)

set(dir "gui/panels/croppanel")
file(GLOB SRC_Gui ${SRC_Gui} ${dir}/*.cpp)
set(dir "gui/panels/mainpanel")
file(GLOB SRC_Gui ${SRC_Gui} ${dir}/*.cpp)
set(dir "gui/panels/sidepanel")
file(GLOB SRC_Gui ${SRC_Gui} ${dir}/*.cpp)

set(dir "gui/viewers")
file(GLOB SRC_Gui ${SRC_Gui} ${dir}/*.cpp)
if(VIDEO_SUPPORT)
    file(GLOB SRC_Gui ${SRC_Gui} ${dir}/playermpv/*.cpp)
else()
    file(GLOB SRC_Gui ${SRC_Gui} ${dir}/playerdummy/*.cpp)
endif()

set(dir "gui/folderview")
file(GLOB SRC_Gui ${SRC_Gui} ${dir}/*.cpp)

# clean up some possible mess from old builds
list(REMOVE_ITEM SRC_Gui */moc_*.cpp)
list(REMOVE_ITEM SRC_Gui moc_*.cpp)
list(REMOVE_ITEM SRC_Gui *_automoc.cpp)
