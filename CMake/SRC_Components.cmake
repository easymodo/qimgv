## SOURCES FOR QIMGV COMPONENTS

set(dir "components/scriptmanager")
file(GLOB SRC_Components ${SRC_Components} ${dir}/*.cpp)

set(dir "components/actionmanager")
file(GLOB SRC_Components ${SRC_Components} ${dir}/*.cpp)

set(dir "components/cache")
file(GLOB SRC_Components ${SRC_Components} ${dir}/*.cpp)

set(dir "components/loader")
file(GLOB SRC_Components ${SRC_Components} ${dir}/*.cpp)

set(dir "components/scaler")
file(GLOB SRC_Components ${SRC_Components} ${dir}/*.cpp)

set(dir "components/thumbnailer")
file(GLOB SRC_Components ${SRC_Components} ${dir}/*.cpp)

set(dir "components/directorymanager/watchers")
if(UNIX)
    file(GLOB SRC_Components ${SRC_Components} ${dir}/*.cpp ${dir}/linux/*.cpp)
elseif(WIN32)
    file(GLOB SRC_Components ${SRC_Components} ${dir}/*.cpp ${dir}/windows/*.cpp)
endif(UNIX)
set(dir "components/directorymanager")
file(GLOB SRC_Components ${SRC_Components} ${dir}/*.cpp)

# clean up some possible mess from old builds
list(REMOVE_ITEM SRC_Components */moc_*.cpp)
list(REMOVE_ITEM SRC_Components moc_*.cpp)
list(REMOVE_ITEM SRC_Components *_automoc.cpp)
