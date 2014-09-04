#include "directorymanager.h"

DirectoryManager::DirectoryManager() :
    currentPosition(-1),
    fileInfo(new FileInfo())
{
    filters << "*.jpg" << "*.jpeg" << "*.png" << "*.gif" << "*.bmp";
    currentDir.setNameFilters(filters);
}

void DirectoryManager::setCurrentDir(QString path) {
    if(currentDir.currentPath() != path) {
        currentDir.setCurrent(path);
        currentDir.setNameFilters(filters);
        fileList = currentDir.entryList();
        currentPosition = -1;
        emit directoryChanged(path);
    }
}

void DirectoryManager::next() {
    if(currentDir.exists() && fileList.length()) {
        if(++currentPosition>=fileList.length()) {
            currentPosition=0;
        }
        QString fileName = currentDir.currentPath()
                        +"/"
                        +fileList.at(currentPosition);
        loadFileInfo(fileName);
    }
}

void DirectoryManager::prev() {
    if(currentDir.exists() && fileList.length()) {
        if(--currentPosition<0) {
            currentPosition=fileList.length()-1;
        }
        QString fileName = currentDir.currentPath()
                        +"/"
                        +fileList.at(currentPosition);
        loadFileInfo(fileName);
    }
}

void DirectoryManager::loadFileInfo(QString path) {
    if(fileInfo != NULL && !fileInfo->inUse) {
        qDebug() << "deleting unused fileInfo";
        delete fileInfo;
    }
    fileInfo = NULL;
    fileInfo = new FileInfo(&path);
    fileInfo->setPositions(currentPosition+1, fileList.length());
}

FileInfo* DirectoryManager::setFile(QString path) {
    loadFileInfo(path);
    qDebug() << fileInfo->getDirPath();
    setCurrentDir(fileInfo->getDirPath());
    currentPosition = fileList.indexOf(fileInfo->getName());
    return getFile();
}

FileInfo* DirectoryManager::getFile() {
    return fileInfo;
}
