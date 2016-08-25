#include "fileinfo.h"

FileInfo::FileInfo(QString path, QObject *parent) : QObject(parent), type(NONE), extension(NULL) {
    setFile(path);
}

FileInfo::~FileInfo() {
}

// ##############################################################
// ####################### PUBLIC METHODS #######################
// ##############################################################

QString FileInfo::directoryPath() {
    return fileInfo.absolutePath();
}

QString FileInfo::filePath() {
    return fileInfo.absoluteFilePath();
}

QString FileInfo::fileName() {
    return fileInfo.fileName();
}

// in KB
int FileInfo::fileSize() {
    return truncf(fileInfo.size()/1024);
}

fileType FileInfo::getType() {
    if(type == NONE) {
        guessType();
    }
    return type;
}

const char *FileInfo::fileExtension() {
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
        type = ANIMATED;
    // webp is incorrectly(?) detected as audio/x-riff on my windows pc
    } else if(mimeName == "audio/x-riff") {
        // in case we encounter an actual audio/x-riff
        if(QString::compare(filePath().split('.').last(), "webp", Qt::CaseInsensitive) == 0) {
            extension = "webp";
            type = ANIMATED;
        }
    // TODO: parse header to find out if it supports animation.
    // treat all webp as animated for now.
    } else if(mimeName == "image/webp") {
        extension = "webp";
        type = ANIMATED;
    } else if(mimeName == "image/bmp") {
        extension = "bmp";
        type = STATIC;
    } else {
        extension = ((QString)fileInfo.fileName().split('.').last()).toStdString().c_str();
        type = STATIC;
    }

}
