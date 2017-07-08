#include "imagefactory.h"

ImageFactory::ImageFactory() {
}

Image *ImageFactory::createImage(QString path) {
    ImageInfo *info = new ImageInfo(path);
    Image *img = NULL;
    if(info->imageType() == NONE) {
        qDebug() << "ImageFactory - could not create image from " << info->filePath();
    } else if(info->imageType() == ANIMATED) {
        img = new ImageAnimated(path);
        img->load();
    } else if(info->imageType() == VIDEO) {
        img = new Video(path);
        img->load();
    } else {
        img = new ImageStatic(path);
        img->load();
    }
    delete info;
    return img;
}

Image *ImageFactory::createImage(ImageInfo *info) {
    Image *img = NULL;
    if(info->imageType() == NONE) {
        qDebug() << "ImageFactory - could not create image from " << info->filePath();
    } else if(info->imageType() == ANIMATED) {
        img = new ImageAnimated(info->filePath());
        img->load();
    } else if(info->imageType() == VIDEO) {
        img = new Video(info->filePath());
        img->load();
    } else {
        img = new ImageStatic(info->filePath());
        img->load();
    }
    delete info;
    return img;
}
