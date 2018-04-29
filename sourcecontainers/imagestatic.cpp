#include "imagestatic.h"
#include <time.h>

ImageStatic::ImageStatic(QString _path)
    : Image(_path)
{
    image = nullptr;
    imageEdited = nullptr;
    load();
}

ImageStatic::ImageStatic(std::unique_ptr<DocumentInfo> _info)
    : Image(std::move(_info))
{
    image = nullptr;
    imageEdited = nullptr;
    load();
}

ImageStatic::~ImageStatic() {
    if(image)
        delete image;
    if(imageEdited)
        delete imageEdited;
}

//load image data from disk
void ImageStatic::load() {
    if(isLoaded()) {
        return;
    }
    image = new QImage();
    image->load(mPath, mDocInfo->extension());
    mLoaded = true;
}

// TODO: add a way to configure compression level?
bool ImageStatic::save(QString destPath) {
    int quality = 95;
    return isEdited()?imageEdited->save(destPath, nullptr, quality):image->save(destPath, nullptr, quality);
}

bool ImageStatic::save() {
    int quality = 95;
    return isEdited()?imageEdited->save(mPath, nullptr, quality):image->save(mPath, nullptr, quality);
}

std::unique_ptr<QPixmap> ImageStatic::getPixmap() {
    std::unique_ptr<QPixmap> pix(new QPixmap());
    isEdited()?pix->convertFromImage(*imageEdited):pix->convertFromImage(*image);
    return pix;
}

const QImage *ImageStatic::getSourceImage() {
    return image;
}

const QImage *ImageStatic::getImage() {
    return isEdited()?imageEdited:image;
}

int ImageStatic::height() {
    return isEdited()?imageEdited->height():image->height();
}

int ImageStatic::width() {
    return isEdited()?imageEdited->width():image->width();
}

QSize ImageStatic::size() {
    return isEdited()?imageEdited->size():image->size();
}

bool ImageStatic::setEditedImage(QImage *imageEditedNew) {
    if(imageEditedNew && imageEditedNew->width() != 0) {
        discardEditedImage();
        imageEdited = imageEditedNew;
        mEdited = true;
        return true;
    } else {
        return false;
    }
}

bool ImageStatic::discardEditedImage() {
    if(imageEdited) {
        delete imageEdited;
        imageEdited = nullptr;
        mEdited = false;
        return true;
    }
    return false;
}
