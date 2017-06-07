#include "thumbnailer.h"

Thumbnailer::Thumbnailer(ImageCache *_cache, QString _path, int _target, int _size, bool _squared) :
    path(_path),
    target(_target),
    size(_size),
    cache(_cache),
    squared(_squared)
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

    th->image = tempImage->generateThumbnail(size, squared);
    if(th->image->size() == QSize(0, 0)) {
        // TODO: wat, why would even it be zero size?
        // anyway, maybe show some error icon instead?
        delete th->image;
        th->image = new QPixmap(size, size);
        th->image->fill(QColor(0,0,0,0));
    }
    th->size = size;
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
    emit thumbnailReady(target, th);
}

Thumbnailer::~Thumbnailer() {
    delete factory;
}
