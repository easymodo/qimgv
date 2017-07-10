#include "imageinfo.h"

ImageInfo::ImageInfo(QString path) : mImageType(NONE), mExtension(NULL) {
    fileInfo.setFile(path);
    if(!fileInfo.isFile()) {
        qDebug() << "FileInfo: cannot open: " << path;
        return;
    }
    lastModified = fileInfo.lastModified();
    detectType();
}

ImageInfo::~ImageInfo() {
}

// ##############################################################
// ####################### PUBLIC METHODS #######################
// ##############################################################

QString ImageInfo::directoryPath() {
    return fileInfo.absolutePath();
}

QString ImageInfo::filePath() {
    return fileInfo.absoluteFilePath();
}

QString ImageInfo::fileName() {
    return fileInfo.fileName();
}

QString ImageInfo::baseName() {
    return fileInfo.baseName();
}

// in KB
int ImageInfo::fileSize() {
    return truncf(fileInfo.size()/1024);
}

ImageType ImageInfo::imageType() {
    if(mImageType == NONE) {
        detectType();
    }
    return mImageType;
}

const char *ImageInfo::extension() {
    return mExtension;
}

// ##############################################################
// ####################### PRIVATE METHODS ######################
// ##############################################################

// detect correct file extension
void ImageInfo::detectType() {
    QMimeDatabase mimeDb;
    QMimeType mimeType = mimeDb.mimeTypeForFile(fileInfo.filePath(), QMimeDatabase::MatchContent);
    QString mimeName = mimeType.name();

    if(mimeName == "video/webm") {
        mExtension = "webm";
        mImageType = ImageType::VIDEO;
    } else if(mimeName == "image/jpeg") {
        mExtension = "jpg";
        mImageType = STATIC;
    } else if(mimeName == "image/png") {
        mExtension = "png";
        mImageType = STATIC;
    } else if(mimeName == "image/gif") {
        mExtension = "gif";
        mImageType = ANIMATED;
    // webp is incorrectly(?) detected as audio/x-riff on my windows pc
    } else if(mimeName == "audio/x-riff") {
        // in case we encounter an actual audio/x-riff
        if(QString::compare(fileInfo.completeSuffix(), "webp", Qt::CaseInsensitive) == 0) {
            mExtension = "webp";
            mImageType = ANIMATED;
        }
    // TODO: parse header to find out if it supports animation.
    // treat all webp as animated for now.
    } else if(mimeName == "image/webp") {
        mExtension = "webp";
        mImageType = ANIMATED;
    } else if(mimeName == "image/bmp") {
        mExtension = "bmp";
        mImageType = STATIC;
    } else {
        mExtension = fileInfo.completeSuffix().toStdString().c_str();
        mImageType = STATIC;
    }
}
