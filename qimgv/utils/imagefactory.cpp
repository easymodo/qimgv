#include "imagefactory.h"

std::shared_ptr<Image> ImageFactory::createImage(QString path) {
    std::unique_ptr<DocumentInfo> docInfo(new DocumentInfo(path));
    std::shared_ptr<Image> img = nullptr;
    if(docInfo->type() == NONE) {
        qDebug() << "ImageFactory: cannot load " << docInfo->filePath();
    } else if(docInfo->type() == ANIMATED) {
        img.reset(new ImageAnimated(move(docInfo)));
    } else if(docInfo->type() == VIDEO) {
        img.reset(new Video(move(docInfo)));
    } else {
        img.reset(new ImageStatic(move(docInfo)));
    }
    return img;
}
