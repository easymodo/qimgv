#include "thumbnailer.h"

Thumbnailer::Thumbnailer(ImageCache *_cache, QString _path, int _target) :
    path(_path),
    target(_target),
    cache(_cache) {
}

void Thumbnailer::run() {
    //TODO: check cache first before creating image here
    ImageFactory *factory = new ImageFactory();
    Image *tempImage = factory->createImage(path);
    Thumbnail *th = new Thumbnail();

    th->image = tempImage->generateThumbnail();
    if(tempImage->getType() == GIF) {
        th->label = "[gif]";
    } else if(tempImage->getType() == VIDEO) {
        th->label = "[webm]";
    }
    if(th->image->size() == QSize(0, 0)) {
        delete th->image;
        th->image = new QPixmap(settings->thumbnailSize(), settings->thumbnailSize());
        th->image->fill(QColor(0,0,0,0));
    }
    th->name = tempImage->getInfo()->getFileName();
    delete tempImage;
    delete factory;

    emit thumbnailReady(target, th);
}
