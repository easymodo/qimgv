#include "fileinfo.h"

FileInfo::FileInfo() : type(NONE), extension(NULL) {
}

FileInfo::FileInfo(QString path, QObject *parent) : QObject(parent), type(NONE), extension(NULL) {
    setFile(path);
}

FileInfo::~FileInfo() {
}

void FileInfo::setFile(QString path) {
    fileInfo.setFile(path);
    if(!fileInfo.isFile()) {
        qDebug() << "Cannot open: " << path;
        return;
    }
    lastModified = fileInfo.lastModified();
    guessType();
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
    return truncf(fileInfo.size() * 100 / (1024 * 1024)) / 100;
}

void FileInfo::guessType() {
    QMimeDatabase mimeDb;
    QMimeType mimeType = mimeDb.mimeTypeForFile(fileInfo.filePath(), QMimeDatabase::MatchContent);
    QString mimeName = mimeType.name();

    if(mimeName == "video/webm") {    // case sensitivity?
        extension = "webm";
        type = fileType::VIDEO;
    } else if(mimeName == "image/jpeg") {
        extension = "jpg";
        type = STATIC;
    } else if(mimeName == "image/png") {
        extension = "png";
        type = STATIC;
    } else if(mimeName == "image/gif") {
        extension = "gif";
        type = GIF;
    } else if(mimeName == "image/bmp") {
        extension = "bmp";
        type = STATIC;
    } else {
        type = STATIC;
    }
}

fileType FileInfo::getType() {
    if(type == NONE) {
        guessType();
    }
    return type;
}

const char *FileInfo::getExtension() {
    return extension;
}
