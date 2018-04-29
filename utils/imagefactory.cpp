#include "imagefactory.h"

Image *ImageFactory::createImage(QString path) {
    std::unique_ptr<DocumentInfo> info(new DocumentInfo(path));
    Image *img = nullptr;
    if(info->type() == NONE) {
        qDebug() << "ImageFactory: unsupported file - " << info->filePath();
    } else if(info->type() == ANIMATED) {
        img = new ImageAnimated(move(info));
    } else if(info->type() == VIDEO) {
        img = new Video(move(info));
    } else {
        img = new ImageStatic(move(info));
    }
    return img;
}
