#include "fileinfo.h"

FileInfo::FileInfo() :
    type(NONE)
{
    inUse = 0;
    positions[0] = 0;
    positions[1] = 0;
}

FileInfo::FileInfo(QString *path) :
    type(NONE)
{
    setFile(*path);
    inUse = 0;
    positions[0] = 0;
    positions[1] = 0;
}

FileInfo::~FileInfo() {
}

void FileInfo::setFile(QString path) {
    fInfo.setFile(path);
    if(!fInfo.isFile()){
        qDebug() << "Cannot open: " << path;
        return;
    }
    QFile file(fInfo.filePath());
    file.open(QIODevice::ReadOnly);

    //read first 2 bytes to determine file format
    QByteArray startingBytes= file.read(2).toHex();
    if(startingBytes == "4749") {
        type = GIF;
    }
    else if(startingBytes == "ffd8"
         || startingBytes == "8950"
         || startingBytes == "424d") {
        type = STATIC;
    }
    lastModified = fInfo.lastModified();
    file.close();
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
