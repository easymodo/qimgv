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
    fInfo.setFile(path);
    if(!fInfo.isFile()){
        qDebug() << "Cannot open: " << path;
        return;
    }
    lastModified = fInfo.lastModified();
    detectType();
}

void FileInfo::detectType() {
    QFile file(fInfo.filePath());
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


QString FileInfo::getDirPath() {
    return fInfo.absolutePath();
}

QString FileInfo::getFilePath() {
    return fInfo.absoluteFilePath();
}

QString FileInfo::getName() {
    return fInfo.fileName();
}

qint64 FileInfo::getSize() {
    return fInfo.size()/1024;
}

QDateTime FileInfo::getLastModified() {
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

void FileInfo::setPositions(int current, int from) {
    positions[0]=current;
    positions[1]=from;
}
int FileInfo::getCurrentPos() {
    return positions[0];
}
int FileInfo::getMaxPos() {
    return positions[1];
}
