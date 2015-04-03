#include "fileinfo.h"

FileInfo::FileInfo() :
    type(NONE)
{
    inUse = 0;
    positions[0] = 0;
    positions[1] = 0;
    width=0;
    height=0;
    extension=NULL;
}

FileInfo::FileInfo(QString *path) :
    type(NONE)
{
    inUse = 0;
    positions[0] = 0;
    positions[1] = 0;
    width=0;
    height=0;
    extension=NULL;
    setFile(*path);
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
    detectType();
}

void FileInfo::detectType() {
    QFile file(fileInfo.filePath());
    file.open(QIODevice::ReadOnly);
    //read first 2 bytes to determine file format
    QByteArray startingBytes= file.read(2).toHex();
    file.close();

    if(startingBytes=="4749") {
        type=GIF;
        extension="GIF";
    }
    else if(startingBytes=="ffd8") {
        type=STATIC;
        extension="JPG";
    }
    else if(startingBytes=="8950") {
        type=STATIC;
        extension="PNG";
    }
    else if(startingBytes=="424d") {
        type=STATIC;
        extension="BMP";
    }
}

int FileInfo::getWidth() const
{
    return width;
}

void FileInfo::setWidth(int value)
{
    width = value;
}

int FileInfo::getHeight() const
{
    return height;
}

void FileInfo::setHeight(int value)
{
    height = value;
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

QDateTime FileInfo::getLastModifiedDate() {
    return lastModified;
}

fileType FileInfo::getType() {
    return type;
}

void FileInfo::setType(fileType _type) {
    type = _type;
}

const char *FileInfo::getExtension() const {
    return extension;
}
