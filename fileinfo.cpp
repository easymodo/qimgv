#include "fileinfo.h"

FileInfo::FileInfo() :
    aspect(1),
    fileNumber(-1)
{
    clear();
}

FileInfo::FileInfo(QString *_path) :
    fileNumber(-1),
    aspect(1),
    type(NONE)
{
    qInfo.setFile(*_path);
}

fileType FileInfo::setFile(QString path) {
    qInfo.setFile(path);
    type = NONE;
    QFile file(qInfo.filePath());
    file.open(QIODevice::ReadOnly);
    //read first 2 bytes to determine file format
    QByteArray startingBytes= file.read(2).toHex();
    qDebug() << startingBytes;
    if(startingBytes == "4749") {
        type = GIF;
    }
    else if(startingBytes == "ffd8"
            || startingBytes == "8950"
            || startingBytes == "424d") {
        type = STATIC;
    }
    return type;
}

void FileInfo::setDimensions(QSize _size) {
    size=_size;
    aspect = (double)size.height()/size.width();
}

void FileInfo::clear() {
    qInfo.setFile("");
    type = NONE;
    aspect = 1;
}

QString FileInfo::getInfo() {
    if(!qInfo.exists() || type == NONE)
        return "No file opened  ";
    QString inf = qInfo.fileName()+"  ("+QString::number(size.width())+" x "+QString::number(size.height())+",  "+QString::number(qInfo.size()/1024)+" KB)  ";
    return inf;
}
