#include "fileinfo.h"

fileInfo::fileInfo() :
    aspectRatio(1),
    fileNumber(-1)
{
    clear();
}

fileInfo::fileInfo(QString *_path) :
    fileNumber(-1),
    aspectRatio(1),
    type(NONE)
{
    qInfo.setFile(*_path);
}

bool fileInfo::setFile(QString *_path) {
    qInfo.setFile(*_path);
    type = NONE;
    return qInfo.isFile();
}

void fileInfo::setDimensions(QSize _size) {
    size=_size;
    aspectRatio = (double)size.height()/size.width();
}

void fileInfo::clear() {
    qInfo.setFile("");
    type = NONE;
    aspectRatio = 1;
}

QString fileInfo::getInfo() {
    if(!qInfo.exists() || type == NONE)
        return "No file opened  ";
    QString inf = qInfo.fileName()+"  ("+QString::number(size.width())+" x "+QString::number(size.height())+",  "+QString::number(qInfo.size()/1024)+" KB)  ";
    return inf;
}
