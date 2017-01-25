#include "thumbnailer.h"

Thumbnailer::Thumbnailer(ImageCache *_cache, QString _path, int _target, bool _squared, long _thumbnailId) :
    path(_path),
    target(_target),
    cache(_cache),
    squared(_squared),
    thumbnailId(_thumbnailId)
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

    th->image = tempImage->generateThumbnail(squared);
    if(th->image->size() == QSize(0, 0)) {
        delete th->image;
        th->image = new QPixmap(settings->thumbnailSize(), settings->thumbnailSize());
        th->image->fill(QColor(0,0,0,0));
    }
    th->name = tempImage->info()->fileName();
    th->label.append(QString::number(tempImage->width()));
    th->label.append("x");
    th->label.append(QString::number(tempImage->height()));
    if(tempImage->type() == ANIMATED) {
        th->label.append(" [a]");
    } else if(tempImage->type() == VIDEO) {
        th->label.append(" [v]");
    }
    if(cached) {
       // tempImage->unlock();
    } else {
        delete tempImage;
    }
    emit thumbnailReady(thumbnailId, th);
}

Thumbnailer::~Thumbnailer() {
    delete factory;
}
