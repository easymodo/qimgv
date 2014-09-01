#include "image.h"

Image::Image() : QObject(), mPath()
{
    pixmap = NULL;
    movie = NULL;
    info = NULL;
}

Image::Image(QString path) : QObject(), mPath(path)
{
    loadImage(path);
    pixmap = NULL;
    movie = NULL;
    info = NULL;
}

//use this constructor
Image::Image(FileInfo *_info) :
    QObject(),
    pixmap(NULL),
    movie(NULL),
    info(_info)
{
    loadImage(info->getFilePath());
}

Image::~Image()
{
    if(info->getType()==STATIC)
        delete pixmap;
    if(info->getType()==GIF)
        delete movie;
    delete info;
    QPixmapCache::clear(); // fuck this
}

void Image::loadImage(QString path)
{
    qDebug() << path;
    if(getType() == GIF) {
        movie = new QMovie(path);
        movie->jumpToNextFrame();
        aspectRatio = (double)movie->currentPixmap().height()/
                movie->currentPixmap().width();
    }
    else if(getType() == STATIC) {
        pixmap = new QPixmap(path);
        aspectRatio = (double)pixmap->height()/
                pixmap->width();
        qDebug() << pixmap->height();
    }
    info->inUse = true;
    qDebug() << aspectRatio;
}

QMovie* Image::getMovie() const
{
    return movie;
}

QPixmap* Image::getPixmap() const
{
    return pixmap;
}

int Image::height() {
    if(info->getType() == GIF) {
        return movie->currentPixmap().height();
    }
    if(info->getType() == STATIC) {
        return pixmap->height();
    }
    else return 1;
}

int Image::width() {
    if(info->getType() == GIF) {
        return movie->currentPixmap().width();
    }
    if(info->getType() == STATIC) {
        return pixmap->width();
    }
    else return 1;
}

QSize Image::size() {
    if(info->getType() == GIF) {
        return movie->currentPixmap().size();
    }
    if(info->getType() == STATIC) {
        return pixmap->size();
    }
    else return QSize(1,1);
}

double Image::getAspect() {
    return aspectRatio;
}

int Image::getType() const
{
    return info->getType();
}

QString Image::getPath() const
{
    return info->getFilePath();
}
