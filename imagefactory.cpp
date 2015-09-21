#include "imagefactory.h"

ImageFactory::ImageFactory() {
}

Image* ImageFactory::createImage(QString path) {
    FileInfo *info = new FileInfo(path);
    Image* img;
    if(info->getType() == GIF) {
        img = new ImageAnimated(path);
    } else if (info->getType() == VIDEO) {
        img = new Video(path);
    } else {
        img = new ImageStatic(path);
    }
    img->load();
    return img;
}
