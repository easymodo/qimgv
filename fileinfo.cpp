#include "fileinfo.h"

FileInfo::FileInfo(QString path, QObject *parent) : QObject(parent), type(NONE), extension(NULL) {
    setFile(path);
}

FileInfo::~FileInfo() {
}

// ##############################################################
// ####################### PUBLIC METHODS #######################
// ##############################################################

QString FileInfo::getDirectoryPath() {
    return fileInfo.absolutePath();
}

QString FileInfo::getFilePath() {
    return fileInfo.absoluteFilePath();
}

QString FileInfo::getFileName() {
    return fileInfo.fileName();
}

// in KB
int FileInfo::getFileSize() {
    return truncf(fileInfo.size()/1024);
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

// ##############################################################
// ####################### PRIVATE METHODS ######################
// ##############################################################

void FileInfo::setFile(QString path) {
    fileInfo.setFile(path);
    if(!fileInfo.isFile()) {
        qDebug() << "Cannot open: " << path;
        return;
    }
    lastModified = fileInfo.lastModified();
    guessType();
}

void FileInfo::guessType() {
    QMimeDatabase mimeDb;
    QMimeType mimeType = mimeDb.mimeTypeForFile(fileInfo.filePath(), QMimeDatabase::MatchContent);
    QString mimeName = mimeType.name();

    if(mimeName == "video/webm") {
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
