#include "imageanimated.h"
#include <time.h>

//use this one
ImageAnimated::ImageAnimated(QString _path)
{
    timer = NULL;
    path = _path;
    loaded = false;
    movie = new QMovie();
    type = STATIC;
}

ImageAnimated::~ImageAnimated()
{
    delete movie;
}

//load image data from disk
void ImageAnimated::load()
{
    mutex.lock();
    info = new FileInfo(path);
    if(isLoaded()) {
        mutex.unlock();
        return;
    }
    guessType();
    movie->setFormat("GIF");
    movie->setFileName(path);
    movie->jumpToFrame(0);
    loaded = true;
    mutex.unlock();
}

void ImageAnimated::unload() {
    mutex.lock();
    if(isLoaded()) {
        delete movie;
        movie = new QMovie();
        loaded = false;
    }
    mutex.unlock();
}

void ImageAnimated::save(QString destinationPath) {
    //TODO
}

void ImageAnimated::save() {
    //TODO
}

QPixmap* ImageAnimated::generateThumbnail() {
    int size = globalSettings->s.value("thumbnailSize", 100).toInt();
    QPixmap *thumbnail = new QPixmap(size, size);
    QPixmap *tmp;
    if(!isLoaded()) {
        tmp = new QPixmap(path);
        *tmp = tmp->scaled(size*2,
                           size*2,
                           Qt::KeepAspectRatioByExpanding,
                           Qt::FastTransformation)
                   .scaled(size,
                           size,
                           Qt::KeepAspectRatioByExpanding,
                           Qt::SmoothTransformation);
    } else {
        tmp = new QPixmap();
        mutex.lock();
        *tmp = movie->currentPixmap()
                .scaled(size*2,
                        size*2,
                        Qt::KeepAspectRatioByExpanding,
                        Qt::FastTransformation)
                .scaled(size,
                        size,
                        Qt::KeepAspectRatio,
                        Qt::SmoothTransformation);
        mutex.unlock();
    }
    QRect target(0, 0, size,size);
    target.moveCenter(tmp->rect().center());
    *thumbnail = tmp->copy(target);
    return thumbnail;
}

// in case of gif returns current frame
QPixmap* ImageAnimated::getPixmap()
{
    QPixmap *pix = new QPixmap();
    if(isLoaded()) {
        *pix = movie->currentPixmap();
    }
    return pix;
}

// todo: const ?
QImage* ImageAnimated::getImage() {
    QImage* ptr = new QImage();
    if(isLoaded()) {
        *ptr = movie->currentImage();
    }
    return ptr;
}

int ImageAnimated::height() {
    if(isLoaded()) {
        return movie->currentImage().height();
    }
    return 0;
}

int ImageAnimated::width() {
    if(isLoaded()) {
        return movie->currentImage().width();
    }
    return 0;
}

QSize ImageAnimated::size() {
    if(isLoaded()) {
        return movie->currentImage().size();
    }
    return QSize(0,0);
}

void ImageAnimated::animationStart() {
    if(isLoaded()) {
        animationStop();
        timer = new QTimer();
        timer->setInterval(movie->nextFrameDelay());
        connect(timer, SIGNAL(timeout()), this, SLOT(frameChangedSlot()), Qt::UniqueConnection);
        timer->start();
    }
}

void ImageAnimated::animationStop() {
    if(isLoaded() && timer) {
        if(timer->isActive())
            timer->stop();
        disconnect(timer, SIGNAL(timeout()), this, SLOT(frameChangedSlot()));
        delete timer;
        timer = NULL;
    }
}

void ImageAnimated::frameChangedSlot() {
    if(!movie->jumpToNextFrame()) {
        movie->jumpToFrame(0);
    }
    QPixmap *pixmap = new QPixmap(movie->currentPixmap());
    emit frameChanged(pixmap);
}

void ImageAnimated::rotate(int grad) {
    mutex.lock();
    if(isLoaded()) {
        //TODO
    }
    mutex.unlock();
}

void ImageAnimated::crop(QRect newRect) {
    mutex.lock();
    if(isLoaded()) {
        // TODO
    }
    mutex.unlock();
}
