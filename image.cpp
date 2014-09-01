#include "image.h"

Image::Image() : QObject(), mPath()
{
    image = NULL;
    movie = NULL;
    info = NULL;
}

Image::Image(QString path) : QObject(), mPath(path)
{
    loadImage(path);
    image = NULL;
    movie = NULL;
    info = NULL;
}

//use this constructor
Image::Image(FileInfo *_info) :
    QObject(),
    image(NULL),
    movie(NULL),
    info(_info)
{
    loadImage(info->getFilePath());
}

Image::~Image()
{
    if(info->getType()==STATIC)
        delete image;
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
        aspectRatio = (double)movie->currentImage().height()/
                movie->currentImage().width();
    }
    else if(getType() == STATIC) {
        image = new QImage(path);
        aspectRatio = (double)image->height()/
                image->width();
    }
    info->inUse = true;
    qDebug() << aspectRatio;
}

QMovie* Image::getMovie() const
{
    return movie;
}

QImage* Image::getImage() const
{
    return image;
}

int Image::height() {
    if(info->getType() == GIF) {
        return movie->currentImage().height();
    }
    if(info->getType() == STATIC) {
        return image->height();
    }
    else return 1;
}

int Image::width() {
    if(info->getType() == GIF) {
        return movie->currentImage().width();
    }
    if(info->getType() == STATIC) {
        return image->width();
    }
    else return 1;
}

QSize Image::size() {
    if(info->getType() == GIF) {
        return movie->currentImage().size();
    }
    if(info->getType() == STATIC) {
        return image->size();
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
