#include "core.h"

Core::Core() :
    QObject(),
    imageLoader(NULL),
    dirManager(NULL),
    currentImage(NULL),
    currentMovie(NULL)
{
}

const ImageCache *Core::getCache() {
    return imageLoader->getCache();
}

void Core::initVariables() {
    cache = new ImageCache();
    dirManager = new DirectoryManager();
    imageLoader = new ImageLoader(dirManager);
}

// misc connections not related to gui
void Core::connectSlots() {
    connect(imageLoader, SIGNAL(loadStarted()),
            this, SLOT(updateInfoString()));
    connect(imageLoader, SIGNAL(loadFinished(Image*, int)),
            this, SLOT(onLoadFinished(Image*, int)));
    connect(this, SIGNAL(thumbnailRequested(int)),
            imageLoader, SLOT(generateThumbnailFor(int)));
    connect(imageLoader, SIGNAL(thumbnailReady(int, const QPixmap*)),
            this, SIGNAL(thumbnailReady(int, const QPixmap*)));
    connect(cache, SIGNAL(initialized(int)), this, SIGNAL(cacheInitialized(int)));
}

void Core::init() {
    initVariables();
    connectSlots();
    imageLoader->setCache(cache);
}

void Core::updateInfoString() {
    QString infoString = "";
    infoString.append(" [ " +
                      QString::number(dirManager->currentPos+1) +
                      "/" +
                      QString::number(dirManager->fileList.length()) +
                      " ]   ");
    if(currentImage) {
        infoString.append(currentImage->getInfo()->getFileName() + "  ");
        infoString.append("(" +
                          QString::number(currentImage->width()) +
                          "x" +
                          QString::number(currentImage->height()) +
                          "  ");
        infoString.append(QString::number(currentImage->getInfo()->getFileSize()) + " MB)");
    }

    emit infoStringChanged(infoString);
}

void Core::rotateImage(int grad) {
    if(currentImage!=NULL) {
        currentImage->rotate(grad);
        updateInfoString();
        emit imageAltered(currentImage->getPixmap());
    }
}

void Core::crop(QRect newRect) {
    if(currentImage) {
        currentImage->crop(newRect);
        updateInfoString();
    }
    emit imageAltered(currentImage->getPixmap());
}

void Core::saveImage(QString path) {
    if(currentImage) {
        currentImage->save(path);
    }
}

void Core::saveImage() {
    if(currentImage) {
        currentImage->save();
    }
}

void Core::setCurrentDir(QString path) {
    dirManager->setCurrentDir(path);
}

void Core::slotNextImage() {
    if(dirManager->containsFiles()) {
        imageLoader->loadNext();
    }
}

void Core::slotPrevImage() {
    if(dirManager->containsFiles()) {
        imageLoader->loadPrev();
    }
}

void Core::loadImage(QString path) {
    if(!path.isEmpty() && dirManager->isValidFile(path)) {
        imageLoader->open(path);
    }
    else {
        qDebug() << "ERROR: invalid file selected.";
    }
}

void Core::loadImageByPos(int pos) {
      imageLoader->open(pos);
}

void Core::onLoadFinished(Image* img, int pos) {
    emit signalUnsetImage();
    stopAnimation();
    currentImage = img;
    emit signalSetImage(currentImage->getPixmap());
    if( currentMovie = dynamic_cast<ImageAnimated*>(currentImage) ) {
        startAnimation();
    }
    emit imageChanged(pos);
    updateInfoString();
}

void Core::rescaleForZoom(QSize newSize) {
    if(currentImage && currentImage->isLoaded()) {
        ImageLib imgLib;
        float sourceSize = currentImage->width()*
                           currentImage->height()/1000000;
        float size = newSize.width()*
                     newSize.height()/1000000;
        QPixmap* pixmap;
        float currentScale = 2.0;
        if(currentScale==1.0) {
            pixmap = currentImage->getPixmap();
        } else {
            pixmap = new QPixmap(newSize);
            if(currentScale<1.0) { // downscale
                if(sourceSize>15) {
                    if(size>10) {
                        // large src, larde dest
                        imgLib.fastScale(pixmap, currentImage->getImage(), newSize, false);
                    } else if(size>4 && size<10){
                        // large src, medium dest
                        imgLib.fastScale(pixmap, currentImage->getImage(), newSize, true);
                    } else {
                        // large src, low dest
                        imgLib.bilinearScale(pixmap, currentImage->getImage(), newSize, true);
                    }
                } else {
                    // low src
                    imgLib.bilinearScale(pixmap, currentImage->getImage(), newSize, true);
                }
            } else {
                if(sourceSize>10) { // upscale
                    // large src
                    imgLib.fastScale(pixmap, currentImage->getImage(), newSize, false);
                } else {
                    // low src
                    imgLib.fastScale(pixmap, currentImage->getImage(), newSize, true);
                }
            }
        }
        emit scalingFinished(pixmap);
    }
}

void Core::startAnimation() {
    if(currentMovie) {
        currentMovie->animationStart();
        connect(currentMovie, SIGNAL(frameChanged(QPixmap*)),
                   this, SIGNAL(frameChanged(QPixmap*)));
    }
}

void Core::stopAnimation() {
    if(currentMovie) {
        currentMovie->animationStop();
        disconnect(currentMovie, SIGNAL(frameChanged(QPixmap*)),
                   this, SIGNAL(frameChanged(QPixmap*)));
    }
}
