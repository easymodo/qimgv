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

QString Image::path() const {
    return mPath;
}

bool Image::isLoaded() const {
    return mLoaded;
}

DocumentType Image::type() const {
    return mDocInfo->type();
}

QString Image::name() const {
    return mDocInfo->fileName();
}

bool Image::isEdited() const {
    return mEdited;
}

int Image::fileSize() const {
    return mDocInfo->fileSize();
}
