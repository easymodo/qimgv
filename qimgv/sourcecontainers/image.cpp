#include "image.h"

Image::Image(QString _path)
    : mDocInfo(new DocumentInfo(_path)),
      mLoaded(false),
      mEdited(false),
      mPath(_path)
{
}

Image::Image(std::unique_ptr<DocumentInfo> _info)
    : mDocInfo(std::move(_info)),
      mLoaded(false),
      mEdited(false),
      mPath(mDocInfo->filePath())
{
}

Image::~Image() {
}

QString Image::filePath() const {
    return mPath;
}

bool Image::isLoaded() const {
    return mLoaded;
}

DocumentType Image::type() const {
    return mDocInfo->type();
}

QString Image::fileName() const {
    return mDocInfo->fileName();
}

QString Image::baseName() const {
    return mDocInfo->baseName();
}

bool Image::isEdited() const {
    return mEdited;
}

qint64 Image::fileSize() const {
    return mDocInfo->fileSize();
}

QDateTime Image::lastModified() const {
    return mDocInfo->lastModified();
}

QMap<QString, QString> Image::getExifTags() {
    return mDocInfo->getExifTags();
}

