/*
 * This is sort-of a main controller of application.
 * It creates and initializes all components, then sets up gui and actions.
 * Most of communication between components go through here.
 *
 */

#include "core.h"

Core::Core()
    : QObject(),
      imageLoader(NULL),
      dirManager(NULL),
      cache(NULL),
      scaler(NULL),
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
    cache = new Cache2();
    imageLoader = new NewLoader(dirManager, cache);
    scaler = new Scaler(cache);
}

void Core::connectComponents() {
    connect(loadingTimer, SIGNAL(timeout()), this, SLOT(onLoadingTimeout()));
    connect(imageLoader, SIGNAL(loadStarted()),
            this, SLOT(onLoadStarted()));
    connect(imageLoader, SIGNAL(loadFinished(Image *, int)),
            this, SLOT(onLoadFinished(Image *, int)));
    //connect(dirManager, SIGNAL(fileRemoved(int)), cache, SLOT(removeAt(int)), Qt::DirectConnection);
    //connect(cache, SIGNAL(itemRemoved(int)), thumbnailPanelWidget, SLOT(removeItemAt(int)), Qt::DirectConnection);
    //connect(dirManager, SIGNAL(directorySortingChanged()), this, SLOT(clearCache()));

    connect(mw, SIGNAL(opened(QString)), this, SLOT(loadImageBlocking(QString)));
    connect(mw, SIGNAL(copyRequested(QString)), this, SLOT(copyFile(QString)));
    connect(mw, SIGNAL(moveRequested(QString)), this, SLOT(moveFile(QString)));

    // thumbnails stuff
    //connect(cache, SIGNAL(initialized(int)), thumbnailPanelWidget, SLOT(fillPanel(int)));
    connect(thumbnailPanelWidget, SIGNAL(thumbnailRequested(int, int)),
            imageLoader, SLOT(generateThumbnailFor(int, int)), Qt::UniqueConnection);
    connect(imageLoader, SIGNAL(thumbnailReady(int, Thumbnail*)),
            thumbnailPanelWidget, SLOT(setThumbnail(int, Thumbnail*)));
    connect(thumbnailPanelWidget, SIGNAL(thumbnailClicked(int)), this, SLOT(openByIndex(int)));
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
    connect(actionManager, SIGNAL(toggleFullscreen()), mw, SLOT(triggerFullscreen()));
    connect(actionManager, SIGNAL(zoomIn()), imageViewer, SLOT(slotZoomIn()));
    connect(actionManager, SIGNAL(zoomOut()), imageViewer, SLOT(slotZoomOut()));
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
    connect(actionManager, SIGNAL(exit()), mw, SLOT(close()));
    connect(actionManager, SIGNAL(removeFile()), this, SLOT(removeFile()));
    connect(actionManager, SIGNAL(copyFile()), mw, SLOT(triggerCopyDialog()));
    connect(actionManager, SIGNAL(moveFile()), mw, SLOT(triggerMoveDialog()));
}

void Core::rotateLeft() {
    rotateByDegrees(-90);
}

void Core::rotateRight() {
    rotateByDegrees(90);
}

void Core::removeFile() {
    if(state.hasActiveImage) {
        if(dirManager->removeAt(state.currentIndex)) {
            thumbnailPanelWidget->removeItemAt(state.currentIndex);
            mw->showMessage("File removed.");
            if(!openByIndexBlocking(state.currentIndex))
                openByIndexBlocking(--state.currentIndex);
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
    if(state.hasActiveImage) {
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
        imageViewer->updateImage(scaled);
    } else {
        delete scaled;
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
    if(!dirManager->containsImages() || dirManager->currentDirectory() != newPath) {
        this->reset();
        dirManager->setDirectory(newPath);
        thumbnailPanelWidget->fillPanel(dirManager->fileCount());
        return true;
    }
    return false;
}

void Core::loadImage(QString filePath, bool blocking) {
    ImageInfo *info = new ImageInfo(filePath);
    if(dirManager->isImage(filePath)) {
        // new directory
        setDirectory(info->directoryPath());
        // try to get image's index
        state.currentIndex = dirManager->indexOf(info->fileName());
        if(blocking)
            imageLoader->openBlocking(state.currentIndex);
        else
            imageLoader->open(state.currentIndex);
    } else if(dirManager->isDirectory(filePath)) {
        this->reset();
        // new directory
        setDirectory(info->directoryPath());
        if(dirManager->containsImages()) {
            // open the first image
            this->openByIndexBlocking(0);
        } else {
            // we got an empty directory; show an error
            mw->showMessage("Directory does not contain images.");
        }
    } else {
        mw->showMessage("Invalid/missing file.");
    }
}

void Core::loadImage(QString filePath) {
    loadImage(filePath, false);
}

void Core::loadImageBlocking(QString filePath) {
    loadImage(filePath, true);
}

bool Core::openByIndex(int index) {
    if(index >= 0 && index < dirManager->fileCount()) {
        state.currentIndex = index;
        imageLoader->open(index);
        return true;
    }
    return false;
}

bool Core::openByIndexBlocking(int index) {
    if(index >= 0 && index < dirManager->fileCount()) {
        state.currentIndex = index;
        imageLoader->openBlocking(state.currentIndex);
        return true;
    }
    return false;
}

void Core::slotNextImage() {
    if(dirManager->containsImages()) {
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
        imageLoader->open(index);
        if(dirManager->checkRange(index + 1))
            imageLoader->preload(index + 1);
    }
}

void Core::slotPrevImage() {
    if(dirManager->containsImages()) {
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
        imageLoader->open(index);
        if(dirManager->checkRange(index - 1))
            imageLoader->preload(index - 1);
    }
}

void Core::onLoadStarted() {
    state.isWaitingForLoader = true;
    updateInfoString();
    loadingTimer->start();
}

void Core::onLoadingTimeout() {
    // TODO: show loading message over MainWindow
}

void Core::onLoadFinished(Image *img, int index) {
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
            auto video = dynamic_cast<Video *>(img);
            showGui(); // workaround for mpv. If we play video while mainwindow is hidden we get black screen.
            viewerWidget->showVideo(video->getClip());
        }
        emit imageIndexChanged(index);
        updateInfoString();
    }
}

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
