/*
 * This is sort-of a main controller of application.
 * It creates and initializes all components, then sets up gui and actions.
 * Most of communication between components go through here.
 *
 */

#include "core.h"

Core::Core()
    : QObject(),
      loader(NULL),
      dirManager(NULL),
      cache(NULL),
      scaler(NULL),
      thumbnailer(NULL),
      infiniteScrolling(false)
{
#ifdef __linux__
    // default value of 128k causes memory fragmentation issues
    // finding this took 3 days of my life
    mallopt(M_MMAP_THRESHOLD, 64000);
#endif
    qRegisterMetaType<ScalerRequest>("ScalerRequest");
    initGui();
    initComponents();
    connectComponents();
    initActions();
    readSettings();
    connect(settings, SIGNAL(settingsChanged()), this, SLOT(readSettings()));
}

void Core::readSettings() {
    infiniteScrolling = settings->infiniteScrolling();
}

void Core::showGui() {
    if(mw && !mw->isVisible())
        mw->showDefault();
}

// create MainWindow and all widgets
void Core::initGui() {
    imageViewer = new ImageViewer();
    videoPlayer = new VideoPlayerGL();
    viewerWidget = new ViewerWidget(imageViewer, videoPlayer);
    mw = new MainWindow(viewerWidget);
    thumbnailPanelWidget = new ThumbnailStrip();
    mw->setPanelWidget(thumbnailPanelWidget);
    mw->hide();
}

void Core::initComponents() {
    loadingTimer = new QTimer();
    loadingTimer->setSingleShot(true);
    loadingTimer->setInterval(500); // TODO: test on slower pc & adjust timeout
    dirManager = new DirectoryManager();
    cache = new Cache();
    loader = new Loader();
    scaler = new Scaler(cache);
    thumbnailer = new Thumbnailer(dirManager);
}

void Core::connectComponents() {
    connect(loadingTimer, SIGNAL(timeout()), this, SLOT(onLoadingTimeout()));
    connect(loader, SIGNAL(loadFinished(Image *)),
            this, SLOT(onLoadFinished(Image *)));

    connect(mw, SIGNAL(opened(QString)), this, SLOT(loadByPathBlocking(QString)));
    connect(mw, SIGNAL(copyRequested(QString)), this, SLOT(copyFile(QString)));
    connect(mw, SIGNAL(moveRequested(QString)), this, SLOT(moveFile(QString)));

    // thumbnails stuff
    connect(thumbnailPanelWidget, SIGNAL(thumbnailRequested(QList<int>, int)),
            thumbnailer, SLOT(generateThumbnailFor(QList<int>, int)), Qt::UniqueConnection);
    connect(thumbnailer, SIGNAL(thumbnailReady(Thumbnail*)),
            this, SLOT(forwardThumbnail(Thumbnail*)));
    connect(thumbnailPanelWidget, SIGNAL(thumbnailClicked(int)), this, SLOT(loadByIndex(int)));
    connect(this, SIGNAL(imageIndexChanged(int)), thumbnailPanelWidget, SLOT(highlightThumbnail(int)));
    connect(imageViewer, SIGNAL(scalingRequested(QSize)), this, SLOT(scalingRequest(QSize)));
    connect(scaler, SIGNAL(scalingFinished(QPixmap*,ScalerRequest)), this, SLOT(onScalingFinished(QPixmap*,ScalerRequest)));
}

void Core::initActions() {
    connect(actionManager, SIGNAL(nextImage()), this, SLOT(slotNextImage()));
    connect(actionManager, SIGNAL(prevImage()), this, SLOT(slotPrevImage()));
    connect(actionManager, SIGNAL(fitAll()), imageViewer, SLOT(setFitAll()));
    connect(actionManager, SIGNAL(fitWidth()), imageViewer, SLOT(setFitWidth()));
    connect(actionManager, SIGNAL(fitNormal()), imageViewer, SLOT(setFitOriginal()));

    connect(actionManager, SIGNAL(fitAll()), mw, SLOT(showMessageFitAll()));
    connect(actionManager, SIGNAL(fitWidth()), mw, SLOT(showMessageFitWidth()));
    connect(actionManager, SIGNAL(fitNormal()), mw, SLOT(showMessageFitOriginal()));

    connect(actionManager, SIGNAL(toggleFitMode()), this, SLOT(switchFitMode()));
    connect(actionManager, SIGNAL(toggleFullscreen()), mw, SLOT(triggerFullScreen()));
    connect(actionManager, SIGNAL(zoomIn()), imageViewer, SLOT(zoomIn()));
    connect(actionManager, SIGNAL(zoomOut()), imageViewer, SLOT(zoomOut()));
    connect(actionManager, SIGNAL(zoomInCursor()), imageViewer, SLOT(zoomInCursor()));
    connect(actionManager, SIGNAL(zoomOutCursor()), imageViewer, SLOT(zoomOutCursor()));
    connect(actionManager, SIGNAL(scrollUp()), imageViewer, SLOT(scrollUp()));
    connect(actionManager, SIGNAL(scrollDown()), imageViewer, SLOT(scrollDown()));
    //connect(actionManager, SIGNAL(resize()), this, SLOT(slotResizeDialog()));
    connect(actionManager, SIGNAL(rotateLeft()), this, SLOT(rotateLeft()));
    connect(actionManager, SIGNAL(rotateRight()), this, SLOT(rotateRight()));
    connect(actionManager, SIGNAL(openSettings()), mw, SLOT(showSettings()));
    //connect(actionManager, SIGNAL(crop()), this, SLOT(slotCrop()));
    //connect(actionManager, SIGNAL(setWallpaper()), this, SLOT(slotSelectWallpaper()));
    connect(actionManager, SIGNAL(open()), mw, SLOT(showOpenDialog()));
    connect(actionManager, SIGNAL(save()), mw, SLOT(showSaveDialog()));
    connect(actionManager, SIGNAL(exit()), this, SLOT(close()));
    connect(actionManager, SIGNAL(closeFullScreenOrExit()), mw, SLOT(closeFullScreenOrExit()));
    connect(actionManager, SIGNAL(removeFile()), this, SLOT(removeFile()));
    connect(actionManager, SIGNAL(copyFile()), mw, SLOT(triggerCopyDialog()));
    connect(actionManager, SIGNAL(moveFile()), mw, SLOT(triggerMoveDialog()));
    connect(actionManager, SIGNAL(jumpToFirst()), this, SLOT(jumpToFirst()));
    connect(actionManager, SIGNAL(jumpToLast()), this, SLOT(jumpToLast()));
}

void Core::rotateLeft() {
    rotateByDegrees(-90);
}

void Core::rotateRight() {
    rotateByDegrees(90);
}

void Core::closeBackgroundTasks() {
    thumbnailer->clearTasks();
    loader->clearTasks();
}

void Core::close() {
    mw->close();
    closeBackgroundTasks();
}

void Core::removeFile() {
    if(state.hasActiveImage) {
        if(dirManager->removeAt(state.currentIndex)) {
            mw->showMessage("File removed.");
            thumbnailPanelWidget->removeItemAt(state.currentIndex);
            if(!dirManager->fileCount()) {
                imageViewer->closeImage();
                mw->setInfoString("No file opened.");
            } else {
                if(!loadByIndexBlocking(state.currentIndex))
                    loadByIndexBlocking(--state.currentIndex);
            }
        } else {
            qDebug() << "Error removing file.";
        }
    }
}

void Core::moveFile(QString destDirectory) {
    if(dirManager->copyTo(destDirectory, state.currentIndex)) {
        removeFile();
        mw->showMessage("File moved to: " + destDirectory);
    } else {
        mw->showMessage("Error moving file to: " + destDirectory);
        qDebug() << "Error moving file to: " << destDirectory;
    }
}

void Core::copyFile(QString destDirectory) {
    if(!dirManager->copyTo(destDirectory, state.currentIndex)) {
        mw->showMessage("Error copying file to: " + destDirectory);
        qDebug() << "Error copying file to: " << destDirectory;
    } else {
        mw->showMessage("File copied to: " + destDirectory);
    }
}

// switch between 1:1 and Fit All
void Core::switchFitMode() {
    if(viewerWidget->fitMode() == FIT_ALL)
        viewerWidget->setFitMode(FIT_ORIGINAL);
    else
        viewerWidget->setFitMode(FIT_ALL);
}

void Core::scalingRequest(QSize size) {
    if(state.hasActiveImage && !state.isWaitingForLoader) {
        cache->lock();
        Image *forScale = cache->get(dirManager->fileNameAt(state.currentIndex));
        if(forScale) {
            QString path = dirManager->filePathAt(state.currentIndex);
            scaler->requestScaled(ScalerRequest(forScale, size, path));
        }
        cache->unlock();
    }
}

void Core::onScalingFinished(QPixmap *scaled, ScalerRequest req) {
    if(state.hasActiveImage /* TODO: a better fix > */ && dirManager->filePathAt(state.currentIndex) == req.string) {
        imageViewer->updateFrame(scaled);
    } else {
        delete scaled;
    }
}

void Core::forwardThumbnail(Thumbnail *thumbnail) {
    int index = dirManager->indexOf(thumbnail->name);
    if(index >= 0) {
        thumbnailPanelWidget->setThumbnail(index, thumbnail);
    } else {
        delete thumbnail;
    }
}

void Core::rotateByDegrees(int degrees) {
    /*
    if(state.currentIndex >= 0) {
        currentImage()->rotate(degrees);
        ImageStatic *staticImage;
        if((staticImage = dynamic_cast<ImageStatic *>(currentImage())) != NULL) {
            viewerWidget->showImage(currentImage()->getPixmap());
        }
        else if ((currentVideo = dynamic_cast<Video *>(currentImage())) != NULL) {
            //emit videoAltered(currentVideo->getClip());
        }
        updateInfoString();
    }
    */
}

void Core::trimCache() {
    QList<QString> list;
    list << dirManager->fileNameAt(state.currentIndex - 1);
    list << dirManager->fileNameAt(state.currentIndex);
    list << dirManager->fileNameAt(state.currentIndex + 1);
    cache->lock();
    cache->trimTo(list);
    cache->unlock();
}

void Core::clearCache() {
    cache->lock();
    cache->clear();
    cache->unlock();
}

void Core::stopPlayback() {
    viewerWidget->stopPlayback();
}

// reset state; clear cache; etc
void Core::reset() {
    state.hasActiveImage = false;
    state.isWaitingForLoader = false;
    state.currentIndex = 0;
    //viewerWidget->unset();
    this->clearCache();
}

bool Core::setDirectory(QString newPath) {
    if(!dirManager->hasImages() || dirManager->currentDirectory() != newPath) {
        this->reset();
        dirManager->setDirectory(newPath);
        thumbnailPanelWidget->fillPanel(dirManager->fileCount());
        return true;
    }
    return false;
}

void Core::loadDirectory(QString path) {
    this->reset();
    // new directory
    setDirectory(path);
    if(dirManager->hasImages()) {
        // open the first image
        this->loadByIndexBlocking(0);
    } else {
        // we got an empty directory; show an error
        mw->showMessage("Directory does not contain supported files.");
    }
}

void Core::loadImage(QString path, bool blocking) {
    ImageInfo *info = new ImageInfo(path);
    // new directory
    setDirectory(info->directoryPath());
    state.currentIndex = dirManager->indexOf(info->fileName());
    onLoadStarted();
    QString nameKey = dirManager->fileNameAt(state.currentIndex);
    // First check if image is already cached. If it is, just display it.
    if(cache->contains(nameKey))
        displayImage(cache->get(nameKey));
    else if(blocking)
        loader->loadBlocking(path);
    else
        loader->loadExclusive(path);
}

void Core::loadByPath(QString path, bool blocking) {
    if(path.startsWith("file://", Qt::CaseInsensitive)) {
        path.remove(0, 7);
    }
    if(dirManager->isImage(path)) {
        loadImage(path, blocking);
    } else if(dirManager->isDirectory(path)) {
        loadDirectory(path);
    } else {
        mw->showMessage("File does not exist or is not supported.");
        qDebug() << "Could not open path: " << path;
    }
}

void Core::loadByPath(QString filePath) {
    loadByPath(filePath, false);
}

void Core::loadByPathBlocking(QString filePath) {
    loadByPath(filePath, true);
}

bool Core::loadByIndex(int index) {
    if(index >= 0 && index < dirManager->fileCount()) {
        state.currentIndex = index;
        onLoadStarted();
        QString nameKey = dirManager->fileNameAt(state.currentIndex);
        // First check if image is already cached. If it is, just display it.
        if(cache->contains(nameKey))
            displayImage(cache->get(nameKey));
        else
            loader->loadExclusive(dirManager->filePathAt(state.currentIndex));
        return true;
    }
    return false;
}

bool Core::loadByIndexBlocking(int index) {
    if(index >= 0 && index < dirManager->fileCount()) {
        state.currentIndex = index;
        onLoadStarted();
        QString nameKey = dirManager->fileNameAt(state.currentIndex);
        // First check if image is already cached. If it is, just display it.
        if(cache->contains(nameKey))
            displayImage(cache->get(nameKey));
        else
            loader->loadExclusive(dirManager->filePathAt(state.currentIndex));
        return true;
    }
    return false;
}

void Core::slotNextImage() {
    if(dirManager->hasImages()) {
        int index = state.currentIndex + 1;
        if(index >= dirManager->fileCount()) {
            if(infiniteScrolling) {
                index = 0;
            } else {
                mw->showMessageDirectoryEnd();
                return;
            }
        }
        state.currentIndex = index;
        onLoadStarted();
        QString nameKey = dirManager->fileNameAt(state.currentIndex);
        // First check if image is already cached. If it is, just display it.
        if(cache->contains(nameKey))
            displayImage(cache->get(nameKey));
        else
            loader->loadExclusive(dirManager->filePathAt(state.currentIndex));
        preload(index + 1);
    }
}

void Core::slotPrevImage() {
    if(dirManager->hasImages()) {
        int index = state.currentIndex - 1;
        if(index < 0) {
            if(infiniteScrolling) {
                index = dirManager->fileCount() - 1;
            }
            else {
                mw->showMessageDirectoryStart();
                return;
            }
        }
        state.currentIndex = index;
        onLoadStarted();
        QString nameKey = dirManager->fileNameAt(state.currentIndex);
        // First check if image is already cached. If it is, just display it.
        if(cache->contains(nameKey))
            displayImage(cache->get(nameKey));
        else
            loader->loadExclusive(dirManager->filePathAt(state.currentIndex));
        preload(index - 1);
    }
}

void Core::jumpToFirst() {
    if(dirManager->hasImages()) {
        this->loadByIndex(0);
    }
}

void Core::jumpToLast() {
    if(dirManager->hasImages()) {
        this->loadByIndex(dirManager->fileCount() - 1);
    }
}

void Core::preload(int index) {
    if(settings->usePreloader() && dirManager->checkRange(index) && !cache->contains(dirManager->fileNameAt(index)))
        loader->load(dirManager->filePathAt(index));
}

void Core::onLoadStarted() {
    state.isWaitingForLoader = true;
    updateInfoString();
    loadingTimer->start();
    trimCache();
}

void Core::onLoadingTimeout() {
    // TODO: show loading message over MainWindow
}



void Core::onLoadFinished(Image *img) {
    int index = dirManager->indexOf(img->info()->fileName());
    bool isRelevant = !(index < state.currentIndex - 1 || index > state.currentIndex + 1);
    QString nameKey = dirManager->fileNameAt(index);

    if(isRelevant) {
        if(!cache->insert(nameKey, img)) {
            delete img;
            img = cache->get(nameKey);
        }
    } else {
        delete img;
    }
    if(index == state.currentIndex) {
        displayImage(img);
    }
}

void Core::displayImage(Image *img) {
    loadingTimer->stop();
    state.isWaitingForLoader = false;
    state.hasActiveImage = true;
    if(img) {
        ImageType type = img->info()->imageType();
        if(type == STATIC) {
            viewerWidget->showImage(img->getPixmap());
        } else if(type == ANIMATED) {
            auto animated = dynamic_cast<ImageAnimated *>(img);
            viewerWidget->showAnimation(animated->getMovie());
        } else if(type == VIDEO) {
            imageViewer->closeImage();
            auto video = dynamic_cast<Video *>(img);
            showGui(); // workaround for mpv. If we play video while mainwindow is hidden we get black screen.
            viewerWidget->showVideo(video->getClip());
        }
        emit imageIndexChanged(state.currentIndex);
        updateInfoString();
    }
}

// TODO: this looks ugly
void Core::updateInfoString() {
    QString infoString = "";
    infoString.append("[ " +
                      QString::number(state.currentIndex + 1) +
                      "/" +
                      QString::number(dirManager->fileCount()) +
                      " ]   ");
    if(!state.isWaitingForLoader) {
        Image* img = cache->get(dirManager->fileNameAt(state.currentIndex));
        QString name, fullName = img->info()->fileName();
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
        infoString.append(QString::number(img->info()->fileSize()) + " KB)");
    }
    mw->setInfoString(infoString);
}
