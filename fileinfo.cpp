#include "fileinfo.h"

FileInfo::FileInfo() : type(NONE) {
}

FileInfo::FileInfo(QString path) : type(NONE) {
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

fileType FileInfo::guessType() {
    QStringList lst;
    QString ext = fileInfo.fileName();
    do {
        lst = ext.split(".");
        ext = lst.last();
    } while( lst.count() > 1 );
    if(ext == "webm" || ext == "gifv") { // case sensitivity?
        return VIDEO;
    }

    QFile file(fileInfo.filePath());
    file.open(QIODevice::ReadOnly);
    //read first 2 bytes to determine file format
    QByteArray startingBytes= file.read(2).toHex();
    file.close();

    if(startingBytes=="4749") {

        return GIF;
    }
    else {
        return STATIC;
    }
}

fileType FileInfo::getType() {
    if(type == NONE) {
        type = guessType();
    }
    return type;
}
