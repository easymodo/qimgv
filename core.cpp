#include "core.h"

Core::Core() :
    QObject(),
    imageLoader(NULL),
    dirManager(NULL),
    currentImageAnimated(NULL),
    currentVideo(NULL) {
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
    if(imageLoader->current) {
        filePath = imageLoader->current->getPath();
    }
    return filePath;
}

// ##############################################################
// ####################### PUBLIC SLOTS #########################
// ##############################################################

void Core::updateInfoString() {
    QString infoString = "";
    infoString.append(" [ " +
                      QString::number(dirManager->currentPos + 1) +
                      "/" +
                      QString::number(dirManager->fileNameList.length()) +
                      " ]   ");
    if(imageLoader->current) {
        infoString.append(imageLoader->current->getInfo()->getFileName() + "  ");
        infoString.append("(" +
                          QString::number(imageLoader->current->width()) +
                          "x" +
                          QString::number(imageLoader->current->height()) +
                          "  ");
        infoString.append(QString::number(imageLoader->current->getInfo()->getFileSize()) + " KB)");
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
    if(imageLoader->current) {
        imageLoader->current->save();
    }
}

void Core::saveImage(QString path) {
    if(imageLoader->current) {
        imageLoader->current->save(path);
    }
}

void Core::setCurrentDir(QString path) {
    dirManager->setCurrentDir(path);
}

void Core::rotateImage(int degrees) {
    if(imageLoader->current != NULL) {
        imageLoader->current->rotate(degrees);
        updateInfoString();
        emit imageAltered(imageLoader->current->getPixmap());
    }
}

void Core::setWallpaper(QRect wpRect) {
    if(imageLoader->current) {
        ImageStatic *staticImage;
        if((staticImage = dynamic_cast<ImageStatic *>(imageLoader->current)) != NULL) {
            QImage *cropped = NULL;
            QRect screenRes = QApplication::desktop()->screenGeometry();
            if(cropped = staticImage->cropped(wpRect, screenRes, true)) {
                QString savePath = QDir::homePath() + "/" + ".wallpaper.jpg";
                cropped->save(savePath, getExtension(savePath), 100);
                WallpaperSetter::setWallpaper(savePath);
                delete cropped;
            }
        }
    }
}

void Core::rescaleForZoom(QSize newSize) {
    if(imageLoader->current && imageLoader->current->isLoaded()) {
        ImageLib imgLib;
        float sourceSize = (float) imageLoader->current->width() *
                           imageLoader->current->height() / 1000000;
        float size = (float) newSize.width() *
                     newSize.height() / 1000000;
        QPixmap *pixmap;
        float currentScale = (float) sourceSize / size;
        if(currentScale == 1.0) {
            pixmap = imageLoader->current->getPixmap();
        } else {
            pixmap = new QPixmap(newSize);
            if(settings->useFastScale()) {
                //imgLib.fastScale(pixmap, imageLoader->current->getPixmap(), newSize, true);
            } else {
                imgLib.bilinearScale(pixmap, imageLoader->current->getPixmap(), newSize, true);
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
    if(imageLoader->current) {
        if((currentImageAnimated = dynamic_cast<ImageAnimated *>(imageLoader->current)) != NULL) {
            currentImageAnimated->animationStop();
            disconnect(currentImageAnimated, SIGNAL(frameChanged(QPixmap *)),
                       this, SIGNAL(frameChanged(QPixmap *)));
        }
        if((currentVideo = dynamic_cast<Video *>(imageLoader->current)) != NULL) {
            emit videoChanged(currentVideo->filePath());
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

    if((currentImageAnimated = dynamic_cast<ImageAnimated *>(img)) != NULL) {
        startAnimation();
    }
    if((currentVideo = dynamic_cast<Video *>(img)) != NULL) {
        emit videoChanged(currentVideo->filePath());
    }
    if(!currentVideo && img) {    //static image
        emit signalSetImage(img->getPixmap());
    }
    emit imageChanged(pos);
    updateInfoString();
    mutex.unlock();
}

void Core::crop(QRect newRect) {
    if(imageLoader->current) {
        ImageStatic *staticImage;
        if((staticImage = dynamic_cast<ImageStatic *>(imageLoader->current)) != NULL) {
            staticImage->crop(newRect);
            updateInfoString();
        }
    }
    emit imageAltered(imageLoader->current->getPixmap());
}
