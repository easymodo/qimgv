#include "fileinfo.h"

FileInfo::FileInfo() {
}

FileInfo::FileInfo(QString path) {
    setFile(path);
}

FileInfo::~FileInfo() {
}

void FileInfo::setFile(QString path) {
    fileInfo.setFile(path);
    if(!fileInfo.isFile()){
        qDebug() << "Cannot open: " << path;
        return;
    }
    lastModified = fileInfo.lastModified();
}

QString FileInfo::getDirectoryPath() {
    return fileInfo.absolutePath();
}

QString FileInfo::getFilePath() {
    return fileInfo.absoluteFilePath();
}

QString FileInfo::getFileName() {
    return fileInfo.fileName();
}

float FileInfo::getFileSize() {
    return truncf(fileInfo.size()*100/(1024*1024))/100;
}

