#include "core.h"

Core::Core(ImageCache *cache) :
    QObject(),
    imageLoader(NULL),
    dirManager(NULL)
{
    initVariables();
    imageLoader->setCache(cache);
    connectSlots();
    initSettings();
}

const ImageCache *Core::getCache() {
    return imageLoader->getCache();
}

void Core::initVariables() {
    dirManager = new DirectoryManager();
    imageLoader = new ImageLoader(dirManager);
    currentImage = NULL;
    currentMovie = NULL;
}

// misc connections not related to gui
void Core::connectSlots() {
    connect(imageLoader, SIGNAL(loadStarted()),
            this, SLOT(updateInfoString()));
    connect(imageLoader, SIGNAL(loadFinished(Image*)),
            this, SLOT(onLoadFinished(Image*)));
}

void Core::initSettings() {
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
   // currentImage = NULL;
    imageLoader->loadNext();
}

void Core::slotPrevImage() {
   // currentImage = NULL;
    imageLoader->loadPrev();
}

void Core::loadImage(QString path) {
    if(!path.isEmpty()) {
    //    currentImage = NULL;
        imageLoader->open(path);
    }
}

void Core::loadImageByPos(int pos) {
    //  currentImage = NULL;
      imageLoader->open(pos);
}

void Core::onLoadFinished(Image* img) {
    emit signalUnsetImage();
    stopAnimation();
    currentImage = img;
    emit signalSetImage(currentImage->getPixmap());
    if( currentMovie = dynamic_cast<ImageAnimated*>(currentImage) ) {
        startAnimation();
    }
    updateInfoString();
}

void Core::rescaleForZoom(QSize newSize) {
    float sourceSize = currentImage->width()*
                       currentImage->height()/1000000;
    float size = newSize.width()*
                 newSize.height()/1000000;
    QPixmap *pixmap;
    float currentScale = 2.0;
    if(currentScale==1.0) {
        pixmap = currentImage->getPixmap();
    } else if(currentScale<1.0) { // downscale
        if(sourceSize>15) {
            if(size>10) {
                // large src, larde dest
                pixmap = ImageLib::fastScale(currentImage->getImage(), newSize, false);
            } else if(size>4 && size<10){
                // large src, medium dest
                pixmap = ImageLib::fastScale(currentImage->getImage(), newSize, true);
            } else {
                // large src, low dest
                pixmap = ImageLib::bilinearScale(currentImage->getImage(), newSize, true);
            }
        } else {
            // low src
            pixmap = ImageLib::bilinearScale(currentImage->getImage(), newSize, true);
        }
    } else {
        if(sourceSize>10) { // upscale
            // large src
            pixmap = ImageLib::fastScale(currentImage->getImage(), newSize, false);
        } else {
            // low src
            pixmap = ImageLib::fastScale(currentImage->getImage(), newSize, true);
        }
    }
    emit scalingFinished(pixmap);
}

void Core::startAnimation() {
    if(currentMovie) {
        currentMovie->animationStart();
        connect(currentMovie, SIGNAL(frameChanged(QPixmap*)),
                   this, SIGNAL(frameChanged(QPixmap*)));
    }
}

void Core::pauseAnimation() {
    if(currentMovie) {
        currentMovie->animationPause();
    }
}

void Core::stopAnimation() {
    if(currentMovie) {
        currentMovie->animationStop();
        disconnect(currentMovie, SIGNAL(frameChanged(QPixmap*)),
                   this, SIGNAL(frameChanged(QPixmap*)));
    }
}
