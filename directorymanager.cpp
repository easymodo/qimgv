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

// full paths array
QStringList DirectoryManager::getFileList() {
    QStringList files;
    QString dirPath = currentDir.absolutePath();
    for(int i=0; i<fileList.length(); i++) {
        files.append(dirPath+"/"+fileList.at(i));
    }
    return files;
}

QFileInfoList DirectoryManager::getFileInfoList() {
    return currentDir.entryInfoList();
}

bool DirectoryManager::existsInCurrentDir(QString file) {
    return fileList.contains(file, Qt::CaseInsensitive);
}

void DirectoryManager::setFile(QString path) {
    FileInfo *info = loadInfo(path);
    setCurrentDir(info->getDirectoryPath());
    currentPos = fileList.indexOf(info->getFileName());
    globalSettings->s.setValue("lastPosition", currentPos);
    //return info;
}

void DirectoryManager::setCurrentPos(int pos) {
    currentPos = pos;
    globalSettings->s.setValue("lastPosition", currentPos);
}

bool DirectoryManager::isValidFile(QString path) {
    QFile file(path);
    QString extension = "nope";
    if(path.contains('.')) {
        extension = "*.";
        extension.append(path.split(".",QString::SkipEmptyParts).at(1));
    }
    if( file.exists() && filters.contains(extension, Qt::CaseInsensitive) ) {
        return true;
    }
    else return false;
}

QString DirectoryManager::currentFileName() {
    return fileList.at(currentPos);
}

QString DirectoryManager::currentDirectory() {
    return currentDir.absolutePath();
}

int DirectoryManager::currentFilePos() {
    return currentPos;
}

bool DirectoryManager::containsFiles() {
    return !fileList.empty();
}

int DirectoryManager::nextPos() {
    if(currentPos<0) {
        currentPos=0;
    } else if(++currentPos>=fileList.length()) {
        currentPos = fileList.length()-1;
    }
    return currentPos;
}

int DirectoryManager::prevPos() {
    if(--currentPos<0) {
        currentPos = 0;
    }
    return currentPos;
}

int DirectoryManager::peekNext(int offset) {
    int pos = currentPos+offset;
    if(pos>=fileList.length()) {
        pos = fileList.length()-1;
    }
    return pos;
}

int DirectoryManager::peekPrev(int offset) {
    int pos = currentPos-offset;
    if(pos<0) {
        pos = 0;
    }
    return pos;
}

int DirectoryManager::lastPos() {
    return fileList.length()-1;
}

FileInfo* DirectoryManager::loadInfo(QString path) {
    FileInfo *info = new FileInfo(path);
    return info;
}
