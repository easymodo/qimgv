#include "thumbnailer.h"

Thumbnailer::Thumbnailer(ImageCache *_cache, QString _path, int _target) :
    path(_path),
    target(_target),
    cache(_cache)
{
    factory = new ImageFactory();
}

void Thumbnailer::run() {
    Image *tempImage;
    Thumbnail *th = new Thumbnail();
    bool cached = false;
    if(cache->isLoaded(target)) {
        tempImage = cache->imageAt(target);
        //tempImage->lock();
        cached = true;
    } else {
        tempImage = factory->createImage(path);
    }

    th->image = tempImage->generateThumbnail();
    if(tempImage->type() == ANIMATED) {
        th->label = "[" + QString::fromLatin1(tempImage->fileInfo->fileExtension()) + "]";
    } else if(tempImage->type() == VIDEO) {
        th->label = "[webm]";
    }
    if(th->image->size() == QSize(0, 0)) {
        delete th->image;
        th->image = new QPixmap(settings->thumbnailSize(), settings->thumbnailSize());
        th->image->fill(QColor(0,0,0,0));
    }
    th->name = tempImage->info()->fileName();

    if(cached) {
       // tempImage->unlock();
    } else {
        delete tempImage;
    }
    emit thumbnailReady(target, th);
}

Thumbnailer::~Thumbnailer() {
    delete factory;
}
