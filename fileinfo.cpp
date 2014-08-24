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

bool FileInfo::setFile(QString *_path) {
    qInfo.setFile(*_path);
    type = NONE;
    return qInfo.isFile();
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
