#include "directorymanager.h"

DirectoryManager::DirectoryManager() :
    currentPos(-1)
{
    filters << "*.jpg" << "*.jpeg" << "*.png" << "*.gif" << "*.bmp";
    QString startDir;
    startDir = globalSettings->s.value("lastDir",
                                       currentDir.homePath()).toString();
    setCurrentDir(startDir);
    currentDir.setNameFilters(filters);
}

void DirectoryManager::setCurrentDir(QString path) {
    if(currentDir.exists()) {
        if(currentDir.path() != path) {
            changePath(path);
        }
    }
    else changePath(path);
}

void DirectoryManager::changePath(QString path) {
    currentDir.setPath(path);
    if(currentDir.isReadable())
        globalSettings->s.setValue("lastDir", path);
    currentDir.setNameFilters(filters);
    currentPos = -1;
    fileList = currentDir.entryList();
    emit directoryChanged(path);
}

FileInfo* DirectoryManager::next() {
    if(fileList.length()) {
        currentPos = nextPos();
        QString filePath = currentDir.filePath(fileList.at(currentPos));
        return setFile(filePath);
    }
}
FileInfo* DirectoryManager::prev() {
    if(fileList.length()) {
        currentPos = prevPos();
        QString filePath = currentDir.filePath(fileList.at(currentPos));
        return setFile(filePath);
    }
}

int DirectoryManager::nextPos() {
    int newPos = currentPos;
    if(++newPos>=fileList.length()) {
        newPos=0;
    }
    return newPos;
}

int DirectoryManager::prevPos() {
    int newPos = currentPos;
    if(--newPos<0) {
        newPos=fileList.length()-1;
    }
    return newPos;
}

FileInfo* DirectoryManager::peekNext() {
    QString path = currentDir.filePath(fileList.at(nextPos()));
    return loadInfo(path);
}

FileInfo* DirectoryManager::peekPrev() {
    QString path = currentDir.filePath(fileList.at(prevPos()));
    return loadInfo(path);
}

FileInfo* DirectoryManager::loadInfo(QString path) {
    FileInfo *info = new FileInfo(&path);
    if(info) {
        info->setPositions(fileList.indexOf(info->getName()), fileList.length());
    }
    return info;
}

FileInfo* DirectoryManager::setFile(QString path) {
    FileInfo *info = loadInfo(path);
    setCurrentDir(info->getDirPath());
    currentPos = fileList.indexOf(info->getName());
    globalSettings->s.setValue("lastPosition", currentPos);
    return info;
}
