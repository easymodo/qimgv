#include "core.h"

Core::Core() :
    QObject(),
    imageLoader(NULL),
    dirManager(NULL),
    currentImageAnimated(NULL),
    currentVideo(NULL)
{
}

const ImageCache *Core::getCache() {
    return imageLoader->getCache();
}

void Core::initVariables() {
    cache = new ImageCache();
    dirManager = new DirectoryManager();
    imageLoader = new NewLoader(dirManager);
}

// misc connections not related to gu
void Core::connectSlots() {
    connect(imageLoader, SIGNAL(loadStarted()),
            this, SLOT(onLoadStarted()));
    connect(imageLoader, SIGNAL(loadFinished(Image*, int)),
            this, SLOT(onLoadFinished(Image*, int)));
    connect(this, SIGNAL(thumbnailRequested(int)),
            imageLoader, SLOT(generateThumbnailFor(int)));
    connect(imageLoader, SIGNAL(thumbnailReady(int, Thumbnail*)),
            this, SIGNAL(thumbnailReady(int, Thumbnail*)));
    connect(cache, SIGNAL(initialized(int)), this, SIGNAL(cacheInitialized(int)), Qt::DirectConnection);
    connect(dirManager, SIGNAL(directorySortingChanged()), imageLoader, SLOT(reinitCacheForced()));
    connect(imageLoader, SIGNAL(currentImageUnloading()),
            this, SLOT(releaseCurrentImage()));
}

QString Core::getCurrentFilePath() {
    QString filePath = "";
    if(imageLoader->current) {
        filePath = imageLoader->current->getPath();
    }
    return filePath;
}

void Core::init() {
    initVariables();
    connectSlots();
    imageLoader->setCache(cache);
}

void Core::onLoadStarted() {
    //if(imageLoader->current) imageLoader->current->lock();
    stopAnimation();
    updateInfoString();
    //if(imageLoader->current) imageLoader->current->unlock();
}

void Core::updateInfoString() {
    QString infoString = "";
    infoString.append(" [ " +
                      QString::number(dirManager->currentPos+1) +
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
        infoString.append(QString::number(imageLoader->current->getInfo()->getFileSize()) + " MB)");
    } else {
        infoString.append(" ...");
    }

    //infoString.append(" >>" + QString::number(cache->currentlyLoadedCount()));

    emit infoStringChanged(infoString);
}

void Core::rotateImage(int grad) {
    if(imageLoader->current!=NULL) {
        imageLoader->current->rotate(grad);
        updateInfoString();
        emit imageAltered(imageLoader->current->getPixmap());
    }
}

void Core::crop(QRect newRect) {
    if(imageLoader->current) {
        ImageStatic* staticImage;
        if((staticImage = dynamic_cast<ImageStatic*>(imageLoader->current)) != NULL) {
            staticImage->crop(newRect);
            updateInfoString();
        }
    }
    emit imageAltered(imageLoader->current->getPixmap());
}

void Core::setWallpaper(QRect wpRect) {
    if(imageLoader->current) {
        ImageStatic* staticImage;
        if((staticImage = dynamic_cast<ImageStatic*>(imageLoader->current)) != NULL) {
            QImage* cropped = NULL;
            QRect screenRes = QApplication::desktop()->screenGeometry();
            if(cropped = staticImage->cropped(wpRect, screenRes, true)) {
                QString savePath = QDir::homePath()+"/"+".wallpaper.jpg";
                cropped->save(savePath, getExtension(savePath), 100);
                WallpaperSetter::setWallpaper(savePath);
                delete cropped;
            }
        }
    }
}

void Core::releaseCurrentImage() {
    if(imageLoader->current == currentImageAnimated != NULL) {
        stopAnimation();
        stopVideo();
    }
}

void Core::saveImage(QString path) {
    if(imageLoader->current) {
        imageLoader->current->save(path);
    }
}

void Core::saveImage() {
    if(imageLoader->current) {
        imageLoader->current->save();
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

void Core::loadImageBlocking(QString path) {
    if(!path.isEmpty() && dirManager->isValidFile(path)) {
        imageLoader->openBlocking(path);
    }
    else {
        qDebug() << "ERROR: invalid file selected.";
    }
}

void Core::loadImageByPos(int pos) {
      imageLoader->open(pos);
}

void Core::onLoadFinished(Image* img, int pos) {
    mutex.lock();
    emit signalUnsetImage();
    if(currentVideo) {
        emit stopVideo();
    }

    if( (currentImageAnimated = dynamic_cast<ImageAnimated*>(imageLoader->current)) != NULL ) {
        startAnimation();
    }
    if ( (currentVideo = dynamic_cast<Video*>(imageLoader->current)) != NULL) {
        emit videoChanged(currentVideo->filePath());
    }
    if(!currentVideo && imageLoader->current) { //static image
        emit signalSetImage(imageLoader->current->getPixmap());
    }
    emit imageChanged(pos);
    updateInfoString();
    mutex.unlock();
}

void Core::rescaleForZoom(QSize newSize) {
    if(imageLoader->current && imageLoader->current->isLoaded()) {
        ImageLib imgLib;
        float sourceSize = (float)imageLoader->current->width()*
                           imageLoader->current->height()/1000000;
        float size = (float)newSize.width()*
                     newSize.height()/1000000;
        QPixmap* pixmap;
        float currentScale = (float)sourceSize/size;
        if(currentScale==1.0) {
            pixmap = imageLoader->current->getPixmap();
        } else {
            pixmap = new QPixmap(newSize);
            if( globalSettings->useFastScale() ) {
                //imgLib.fastScale(pixmap, imageLoader->current->getPixmap(), newSize, true);
            }
            else {
                imgLib.bilinearScale(pixmap, imageLoader->current->getPixmap(), newSize, true);
            }
        }
        emit scalingFinished(pixmap);
    }
}

void Core::startAnimation() {
    if(imageLoader->current && currentImageAnimated) {
        currentImageAnimated->animationStart();
        connect(currentImageAnimated, SIGNAL(frameChanged(QPixmap*)),
                   this, SIGNAL(frameChanged(QPixmap*)), Qt::UniqueConnection);
    }
}

void Core::stopAnimation() {
    if(imageLoader->current && currentImageAnimated) {
        currentImageAnimated->animationStop();
        disconnect(currentImageAnimated, SIGNAL(frameChanged(QPixmap*)),
                   this, SIGNAL(frameChanged(QPixmap*)));
    }
}
