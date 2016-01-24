#include "core.h"

Core::Core() :
    QObject(),
    imageLoader(NULL),
    dirManager(NULL),
    currentImageAnimated(NULL),
    currentVideo(NULL),
    currentImagePos(0) {
}

// ##############################################################
// ####################### PUBLIC METHODS #######################
// ##############################################################

void Core::init() {
    initVariables();
    connectSlots();
    imageLoader->setCache(cache);
}

QString Core::getCurrentFilePath() {
    QString filePath = "";
    if(currentImage()) {
        filePath = currentImage()->getPath();
    }
    return filePath;
}

int Core::imageCount() {
    if(!dirManager)
        return 0;

    return dirManager->fileCount();
}

// ##############################################################
// ####################### PUBLIC SLOTS #########################
// ##############################################################

void Core::updateInfoString() {
    Image* img = currentImage();
    QString infoString = "";
    infoString.append(" [ " +
                      QString::number(dirManager->currentPos + 1) +
                      "/" +
                      QString::number(dirManager->fileNameList.length()) +
                      " ]   ");
    if(img) {
        QString name, fullName = img->getInfo()->getFileName();
        if(fullName.size()>95) {
            name = fullName.left(95);
            name.append(" (...) ");
            name.append(fullName.right(12));
        } else {
            name = fullName;
        }
        infoString.append(name + "  ");
        infoString.append("(" +
                          QString::number(img->width()) +
                          "x" +
                          QString::number(img->height()) +
                          "  ");
        infoString.append(QString::number(img->getInfo()->getFileSize()) + " KB)");
    }

    //infoString.append(" >>" + QString::number(cache->currentlyLoadedCount()));
    emit infoStringChanged(infoString);
}

void Core::loadImage(QString path) {
    if(!path.isEmpty() && dirManager->isImage(path)) {
        imageLoader->open(path);
    } else {
        qDebug() << "ERROR: invalid file selected.";
    }
}

void Core::loadImageBlocking(QString path) {
    if(!path.isEmpty() && dirManager->isImage(path)) {
        imageLoader->openBlocking(path);
    } else {
        qDebug() << "ERROR: invalid file selected.";
    }
}

void Core::loadImageByPos(int pos) {
    imageLoader->open(pos);
}

void Core::slotNextImage() {
    if(dirManager->containsImages()) {
        imageLoader->loadNext();
    }
}

void Core::slotPrevImage() {
    if(dirManager->containsImages()) {
        imageLoader->loadPrev();
    }
}

void Core::saveImage() {
    if(currentImage()) {
        currentImage()->save();
    }
}

void Core::saveImage(QString path) {
    if(currentImage()) {
        currentImage()->save(path);
    }
}

void Core::setCurrentDir(QString path) {
    dirManager->setCurrentDir(path);
}

void Core::rotateImage(int degrees) {
    if(currentImage() != NULL) {
        currentImage()->rotate(degrees);
        ImageStatic *staticImage;
        if((staticImage = dynamic_cast<ImageStatic *>(currentImage())) != NULL) {
            emit imageAltered(currentImage()->getPixmap());
        }
        else if ((currentVideo = dynamic_cast<Video *>(currentImage())) != NULL) {
            emit videoAltered(currentVideo->getClip());
        }
        updateInfoString();
    }
}

void Core::setWallpaper(QRect wpRect) {
    if(currentImage()) {
        ImageStatic *staticImage;
        if((staticImage = dynamic_cast<ImageStatic *>(currentImage())) != NULL) {
            QImage *cropped = NULL;
            QRect screenRes = QApplication::desktop()->screenGeometry();
            if(cropped = staticImage->cropped(wpRect, screenRes, true)) {
                QString savePath = QDir::homePath() + "/" + ".wallpaper.png";
                cropped->save(savePath, getExtension(savePath), 100);
                WallpaperSetter::setWallpaper(savePath);
                delete cropped;
            }
        }
    }
}

void Core::rescaleForZoom(QSize newSize) {
    if(currentImage() && currentImage()->isLoaded()) {
        ImageLib imgLib;
        float sourceSize = (float) currentImage()->width() *
                           currentImage()->height() / 1000000;
        float size = (float) newSize.width() *
                     newSize.height() / 1000000;
        QPixmap *pixmap;
        float currentScale = (float) sourceSize / size;
        if(currentScale == 1.0) {
            pixmap = currentImage()->getPixmap();
        } else {
            pixmap = new QPixmap(newSize);
            if(settings->useFastScale()) {
                //imgLib.fastScale(pixmap, currentImage()->getPixmap(), newSize, true);
            } else {
                imgLib.bilinearScale(pixmap, currentImage()->getPixmap(), newSize, true);
            }
        }
        emit scalingFinished(pixmap);
    }
}

void Core::startAnimation() {
    if(currentImageAnimated) {
        currentImageAnimated->animationStart();
        connect(currentImageAnimated, SIGNAL(frameChanged(QPixmap *)),
                this, SIGNAL(frameChanged(QPixmap *)), Qt::UniqueConnection);
    }
}

void Core::stopAnimation() {
    if(currentImage()) {
        if((currentImageAnimated = dynamic_cast<ImageAnimated *>(currentImage())) != NULL) {
            currentImageAnimated->animationStop();
            disconnect(currentImageAnimated, SIGNAL(frameChanged(QPixmap *)),
                       this, SIGNAL(frameChanged(QPixmap *)));
        }
        if((currentVideo = dynamic_cast<Video *>(currentImage())) != NULL) {
            emit stopVideo();
        }
    }
}

// ##############################################################
// ####################### PRIVATE METHODS ######################
// ##############################################################

void Core::initVariables() {
    cache = new ImageCache();
    dirManager = new DirectoryManager();
    imageLoader = new NewLoader(dirManager);
}

void Core::connectSlots() {
    connect(imageLoader, SIGNAL(loadStarted()),
            this, SLOT(onLoadStarted()));
    connect(imageLoader, SIGNAL(loadFinished(Image *, int)),
            this, SLOT(onLoadFinished(Image *, int)));
    connect(this, SIGNAL(thumbnailRequested(int)),
            imageLoader, SLOT(generateThumbnailFor(int)));
    connect(imageLoader, SIGNAL(thumbnailReady(int, Thumbnail *)),
            this, SIGNAL(thumbnailReady(int, Thumbnail *)));
    connect(cache, SIGNAL(initialized(int)), this, SIGNAL(cacheInitialized(int)), Qt::DirectConnection);
    connect(dirManager, SIGNAL(directorySortingChanged()), imageLoader, SLOT(reinitCacheForced()));
}

Image* Core::currentImage() {
    return cache->imageAt(currentImagePos);
}

// ##############################################################
// ####################### PRIVATE SLOTS ########################
// ##############################################################

void Core::onLoadStarted() {
    updateInfoString();
}

void Core::onLoadFinished(Image *img, int pos) {
    mutex.lock();
    emit signalUnsetImage();

    stopAnimation();
    currentImagePos = pos;

    if((currentImageAnimated = dynamic_cast<ImageAnimated *>(img)) != NULL) {
        startAnimation();
    }    
    if((currentVideo = dynamic_cast<Video *>(img)) != NULL) {
        emit videoChanged(currentVideo->getClip());
    }
    if(!currentVideo && img) {    //static image
        emit signalSetImage(img->getPixmap());
    }

    emit imageChanged(pos);
    updateInfoString();
    mutex.unlock();
}

void Core::crop(QRect newRect) {
    if(currentImage()) {
        ImageStatic *staticImage;
        if((staticImage = dynamic_cast<ImageStatic *>(currentImage())) != NULL) {
            staticImage->crop(newRect);
            updateInfoString();
            emit imageAltered(currentImage()->getPixmap());
        }
        else if ((currentVideo = dynamic_cast<Video *>(currentImage())) != NULL) {
            currentVideo->crop(newRect);
            updateInfoString();
            emit videoAltered(currentVideo->getClip());
        }
    }

}
