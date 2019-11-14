## SOURCES FOR QIMGV CONTAINER CLASSES

set(dir "sourcecontainers")
file(GLOB SRC_SourceContainers ${SRC_SourceContainers} ${dir}/*.cpp)

# clean up some possible mess from old builds
list(REMOVE_ITEM SRC_SourceContainers */moc_*.cpp)
list(REMOVE_ITEM SRC_SourceContainers moc_*.cpp)
list(REMOVE_ITEM SRC_SourceContainers *_automoc.cpp)
