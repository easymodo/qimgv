#include "image.h"
#include <time.h>

Image::Image() : QObject(), mPath()
{
    image = NULL;
    movie = NULL;
    info = NULL;
}

Image::Image(QString path) : QObject(), mPath(path)
{
    image = NULL;
    movie = NULL;
    info = NULL;
}

//use this constructor
Image::Image(FileInfo _info) :
    QObject(),
    image(NULL),
    movie(NULL),
    info(_info)
{
}

Image::~Image()
{
    if(info.getType()==STATIC)
        delete image;
    if(info.getType()==GIF)
        delete movie;
}

//load image data from disk
void Image::loadImage()
{
    QString path = info.getFilePath();
    if(info.getType()!=NONE)  {
        if(getType() == GIF) {
            movie = new QMovie(path);
            movie->jumpToFrame(0);
            aspectRatio = (float)movie->currentImage().height()/
                    movie->currentImage().width();
        }
        else if(getType() == STATIC) {
            int time = clock();
            QImage *tmp = new QImage(path);
            image = new QImage();
            *image = tmp->convertToFormat(QImage::Format_ARGB32_Premultiplied);
            delete tmp;
         //   qDebug() << "format: " << image->format();
            aspectRatio = (float)image->height()/
                    image->width();
        }
        info.inUse = true;
    }
}

bool Image::isInUse() {
    return inUseFlag;
}

void Image::setInUse(bool arg) {
    inUseFlag=arg;
}

int Image::ramSize() {
    if(getType() == GIF) {
        return 1; //later
    }
    else if(getType() == STATIC) {
        return image->byteCount()/(1024*1024);
    }
    else return 0;
}

QMovie* Image::getMovie()
{
    return movie;
}

QImage* Image::getImage()
{
    return image;
}

int Image::height() {
    if(info.getType() == GIF) {
        return movie->currentImage().height();
    }
    if(info.getType() == STATIC) {
        return image->height();
    }
    else return 1;
}

int Image::width() {
    if(info.getType() == GIF) {
        return movie->currentImage().width();
    }
    if(info.getType() == STATIC) {
        return image->width();
    }
    else return 1;
}

QSize Image::size() {
    if(info.getType() == GIF) {
        return movie->currentImage().size();
    }
    if(info.getType() == STATIC) {
        return image->size();
    }
    else return QSize(1,1);
}

float Image::getAspect() {
    return aspectRatio;
}

int Image::getType()
{
    return info.getType();
}

QString Image::getPath()
{
    return info.getFilePath();
}

qint64 Image::getSize() {
    return info.getSize();
}

QString Image::getName() {
    return info.getName();
}

FileInfo Image::getInfo() {
    return info;
}

// returns true if files are identical
// checks filename, size, date
// does not check actual file contents
bool Image::compare(Image* another) {
    if(getSize() == another->getSize() &&
       info.getLastModified() == another->getInfo().getLastModified() &&
       getName() == another->getName() ) {
        return true;
    }
    return false;
}
