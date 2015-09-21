#include "imagefactory.h"

ImageFactory::ImageFactory() {
}

Image* ImageFactory::createImage(QString path) {
    FileInfo *info = new FileInfo(path);
    Image* img;
    if(info->getType() == GIF) {
        img = new ImageAnimated(info);
    } else if (info->getType() == VIDEO) {
        img = new Video(info);
    } else {
        img = new ImageStatic(info);
    }
    img->load();
    return img;
}
