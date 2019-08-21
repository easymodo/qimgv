## SOURCES FOR QIMGV UTILITY CLASSES

set(dir "utils")
file(GLOB SRC_Utils ${SRC_Utils} ${dir}/*.cpp)

# clean up some possible mess from old builds
list(REMOVE_ITEM SRC_Utils */moc_*.cpp)
list(REMOVE_ITEM SRC_Utils moc_*.cpp)
list(REMOVE_ITEM SRC_Utils *_automoc.cpp)
