#include "imagefactory.h"

ImageFactory::ImageFactory() {
}

Image *ImageFactory::createImage(QString path) {
    ImageInfo *info = new ImageInfo(path);
    Image *img;
    if(info->imageType() == ANIMATED) {
        img = new ImageAnimated(path);
    } else if(info->imageType() == VIDEO) {
        img = new Video(path);
    } else {
        img = new ImageStatic(path);
    }
    img->load();
    delete info;
    return img;
}

Image *ImageFactory::createImage(ImageInfo *info) {
    Image *img;
    if(info->imageType() == ANIMATED) {
        img = new ImageAnimated(info->filePath());
    } else if(info->imageType() == VIDEO) {
        img = new Video(info->filePath());
    } else {
        img = new ImageStatic(info->filePath());
    }
    img->load();
    delete info;
    return img;
}
