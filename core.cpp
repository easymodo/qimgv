/*
 * This is sort-of a main controller of application.
 * It creates and initializes all components, then sets up gui and actions.
 * Most of communication between components go through here.
 *
 */

#include "core.h"

Core::Core()
    : QObject(),
      infiniteScrolling(false),
      loader(nullptr),
      dirManager(nullptr),
      cache(nullptr),
      scaler(nullptr),
      thumbnailer(nullptr)
{
#ifdef __GLIBC__
    // default value of 128k causes memory fragmentation issues
    // finding this took 3 days of my life
    mallopt(M_MMAP_THRESHOLD, 64000);
#endif
    qRegisterMetaType<ScalerRequest>("ScalerRequest");
    qRegisterMetaType<std::shared_ptr<Image>>("std::shared_ptr<Image>");
    qRegisterMetaType<std::shared_ptr<Thumbnail>>("std::shared_ptr<Thumbnail>");
    initGui();
    initComponents();
    connectComponents();
    initActions();
    readSettings();
    connect(settings, SIGNAL(settingsChanged()), this, SLOT(readSettings()));

    QVersionNumber lastVersion = settings->lastVersion();
    // If we get (0,0,0) then it is a new install; no need to run update logic.
    // There shouldn't be any weirdness if you update 0.6.3 -> 0.7
    // In addition there is a firstRun flag.
    if(appVersion > lastVersion && lastVersion != QVersionNumber(0,0,0))
        onUpdate();
    if(settings->firstRun())
        onFirstRun();
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
    mw = new MainWindow();
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
    connect(loader, SIGNAL(loadFinished(std::shared_ptr<Image>)),
            this, SLOT(onLoadFinished(std::shared_ptr<Image>)));
    connect(loader, SIGNAL(loadFailed(QString)),
            this, SLOT(onLoadFailed(QString)));

    connect(mw, SIGNAL(opened(QString)), this, SLOT(loadByPathBlocking(QString)));
    connect(mw, SIGNAL(copyRequested(QString)), this, SLOT(copyFile(QString)));
    connect(mw, SIGNAL(moveRequested(QString)), this, SLOT(moveFile(QString)));
    connect(mw, SIGNAL(resizeRequested(QSize)), this, SLOT(resize(QSize)));
    connect(mw, SIGNAL(cropRequested(QRect)), this, SLOT(crop(QRect)));
    connect(mw, SIGNAL(saveAsClicked()), this, SLOT(requestSavePath()));
    connect(mw, SIGNAL(saveRequested()), this, SLOT(saveImageToDisk()));
    connect(mw, SIGNAL(saveRequested(QString)), this, SLOT(saveImageToDisk(QString)));
    connect(mw, SIGNAL(discardEditsRequested()), this, SLOT(discardEdits()));
    connect(this, SIGNAL(imageIndexChanged(int)), mw, SLOT(setupSidePanelData()));

    connect(mw, SIGNAL(thumbnailRequested(QList<int>, int)),
            thumbnailer, SLOT(generateThumbnailFor(QList<int>, int)), Qt::UniqueConnection);

    connect(thumbnailer, SIGNAL(thumbnailReady(std::shared_ptr<Thumbnail>)),
            this, SLOT(forwardThumbnail(std::shared_ptr<Thumbnail>)));

    connect(this, SIGNAL(imageIndexChanged(int)),
            mw, SIGNAL(selectThumbnail(int)));

    connect(mw, SIGNAL(thumbnailPressed(int)),
            this, SLOT(loadByIndex(int)));

    // scaling
    connect(mw, SIGNAL(scalingRequested(QSize)),
            this, SLOT(scalingRequest(QSize)));
    connect(scaler, SIGNAL(scalingFinished(QPixmap*,ScalerRequest)),
            this, SLOT(onScalingFinished(QPixmap*,ScalerRequest)));
    // filesystem changes
    connect(dirManager, SIGNAL(fileRemovedAt(int)), this, SLOT(onFileRemoved(int)));
    connect(dirManager, SIGNAL(fileAddedAt(int)), this, SLOT(onFileAdded(int)));
}

void Core::initActions() {
    connect(actionManager, SIGNAL(nextImage()), this, SLOT(nextImage()));
    connect(actionManager, SIGNAL(prevImage()), this, SLOT(prevImage()));
    connect(actionManager, SIGNAL(fitWindow()), mw, SLOT(fitWindow()));
    connect(actionManager, SIGNAL(fitWidth()), mw, SLOT(fitWidth()));
    connect(actionManager, SIGNAL(fitNormal()), mw, SLOT(fitOriginal()));
    connect(actionManager, SIGNAL(toggleFitMode()), mw, SLOT(switchFitMode()));
    connect(actionManager, SIGNAL(toggleFullscreen()), mw, SLOT(triggerFullScreen()));
    connect(actionManager, SIGNAL(zoomIn()), mw, SIGNAL(zoomIn()));
    connect(actionManager, SIGNAL(zoomOut()), mw, SIGNAL(zoomOut()));
    connect(actionManager, SIGNAL(zoomInCursor()), mw, SIGNAL(zoomInCursor()));
    connect(actionManager, SIGNAL(zoomOutCursor()), mw, SIGNAL(zoomOutCursor()));
    connect(actionManager, SIGNAL(scrollUp()), mw, SIGNAL(scrollUp()));
    connect(actionManager, SIGNAL(scrollDown()), mw, SIGNAL(scrollDown()));
    connect(actionManager, SIGNAL(scrollLeft()), mw, SIGNAL(scrollLeft()));
    connect(actionManager, SIGNAL(scrollRight()), mw, SIGNAL(scrollRight()));
    connect(actionManager, SIGNAL(resize()), this, SLOT(showResizeDialog()));
    connect(actionManager, SIGNAL(flipH()), this, SLOT(flipH()));
    connect(actionManager, SIGNAL(flipV()), this, SLOT(flipV()));
    connect(actionManager, SIGNAL(rotateLeft()), this, SLOT(rotateLeft()));
    connect(actionManager, SIGNAL(rotateRight()), this, SLOT(rotateRight()));
    connect(actionManager, SIGNAL(openSettings()), mw, SLOT(showSettings()));
    connect(actionManager, SIGNAL(crop()), this, SLOT(toggleCropPanel()));
    //connect(actionManager, SIGNAL(setWallpaper()), this, SLOT(slotSelectWallpaper()));
    connect(actionManager, SIGNAL(open()), mw, SLOT(showOpenDialog()));
    connect(actionManager, SIGNAL(save()), this, SLOT(saveImageToDisk()));
    connect(actionManager, SIGNAL(saveAs()), this, SLOT(requestSavePath()));
    connect(actionManager, SIGNAL(exit()), this, SLOT(close()));
    connect(actionManager, SIGNAL(closeFullScreenOrExit()), mw, SLOT(closeFullScreenOrExit()));
    connect(actionManager, SIGNAL(removeFile()), this, SLOT(removeFilePermanent()));
    connect(actionManager, SIGNAL(moveToTrash()), this, SLOT(moveToTrash()));
    connect(actionManager, SIGNAL(copyFile()), mw, SLOT(triggerCopyOverlay()));
    connect(actionManager, SIGNAL(moveFile()), mw, SLOT(triggerMoveOverlay()));
    connect(actionManager, SIGNAL(jumpToFirst()), this, SLOT(jumpToFirst()));
    connect(actionManager, SIGNAL(jumpToLast()), this, SLOT(jumpToLast()));
    connect(actionManager, SIGNAL(runScript(const QString&)), this, SLOT(runScript(const QString&)));
    connect(actionManager, SIGNAL(pauseVideo()), mw, SIGNAL(pauseVideo()));
    connect(actionManager, SIGNAL(seekVideo()), mw, SIGNAL(seekVideoRight()));
    connect(actionManager, SIGNAL(seekBackVideo()), mw, SIGNAL(seekVideoLeft()));
    connect(actionManager, SIGNAL(frameStep()), mw, SIGNAL(frameStep()));
    connect(actionManager, SIGNAL(frameStepBack()), mw, SIGNAL(frameStepBack()));
    connect(actionManager, SIGNAL(folderView()), mw, SIGNAL(enableFolderView()));
    connect(actionManager, SIGNAL(documentView()), mw, SIGNAL(enableDocumentView()));
    connect(actionManager, SIGNAL(toggleFolderView()), mw, SIGNAL(toggleFolderView()));
}

void Core::onUpdate() {
    QVersionNumber lastVer = settings->lastVersion();
    actionManager->resetDefaultsFromVersion(lastVer);
    actionManager->saveShortcuts();
    settings->setLastVersion(appVersion);
    qDebug() << "Updated: " << settings->lastVersion().toString() << ">" << appVersion.toString();
    // TODO: finish changelogs
    //if(settings->showChangelogs())
    //    mw->showChangelogWindow();
    //else
        mw->showMessage("Updated: "+settings->lastVersion().toString()+" > "+appVersion.toString());

}

void Core::onFirstRun() {
    //mw->showSomeSortOfWelcomeScreen();
    mw->showMessage("Welcome to qimgv version " + appVersion.toString() + "!", 3000);
    settings->setFirstRun(false);
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

void Core::removeFilePermanent() {
    if(state.hasActiveImage)
        removeFilePermanent(state.currentIndex);
}

void Core::removeFilePermanent(int index) {
    removeFile(index, false);
}

void Core::moveToTrash() {
    if(state.hasActiveImage)
        moveToTrash(state.currentIndex);
}

void Core::moveToTrash(int index) {
    removeFile(index, true);
}

// removes file at specified index within current directory
void Core::removeFile(int index, bool trash) {
    if(index < 0 || index >= dirManager->fileCount())
        return;
    QString fileName = dirManager->fileNameAt(index);
    if(dirManager->removeAt(index, trash)) {
        QString msg = trash?"Moved to trash: ":"File removed: ";
        mw->showMessage(msg + fileName);
    }
}

void Core::onFileRemoved(int index) {
    mw->removeThumbnail(index);
    // removing current file. try switching to another
    if(state.currentIndex == index) {
        if(!dirManager->fileCount()) {
            mw->closeImage();
            mw->setInfoString("No file opened.");
        } else {
            if(!loadByIndexBlocking(state.currentIndex))
                loadByIndexBlocking(--state.currentIndex);
        }
    }
}

void Core::onFileAdded(int index) {
    mw->addThumbnail(index);
}

void Core::moveFile(QString destDirectory) {
    QFile file(destDirectory+"/"+dirManager->fileNameAt(state.currentIndex));
    if(file.exists()) {
        mw->showMessage("Error: file already exists.");
        return;
    }
    if(dirManager->copyTo(destDirectory, state.currentIndex)) {
        removeFilePermanent();
        mw->showMessage("File moved to: " + destDirectory);
    } else {
        mw->showMessage("Error moving file to: " + destDirectory);
        qDebug() << "Error moving file to: " << destDirectory;
    }
}

void Core::copyFile(QString destDirectory) {
    QFileInfo file(destDirectory + "/" + dirManager->fileNameAt(state.currentIndex));
    if(file.exists()) {
        if(file.isFile())
            mw->showMessage("Error: file already exists.");
        return;
    }
    if(!dirManager->copyTo(destDirectory, state.currentIndex)) {
        mw->showMessage("Error copying file to: " + destDirectory);
        qDebug() << "Error copying file to: " << destDirectory;
    } else {
        mw->showMessage("File copied to: " + destDirectory);
    }
}

void Core::toggleCropPanel() {
    if(mw->isCropPanelActive()) {
        mw->triggerCropPanel();
    } else if(state.hasActiveImage) {
        mw->triggerCropPanel();
    }
}

void Core::requestSavePath() {
    if(state.hasActiveImage) {
        mw->showSaveDialog(dirManager->filePathAt(state.currentIndex));
    }
}

void Core::showResizeDialog() {
    if(state.hasActiveImage) {
        QString nameKey = dirManager->fileNameAt(state.currentIndex);
        mw->showResizeDialog(cache->get(nameKey)->size());
    }
}

// TODO: simplify. too much copypasted code
// use default bilinear for now
void Core::resize(QSize size) {
    if(state.hasActiveImage) {
        QString nameKey = dirManager->fileNameAt(state.currentIndex);
        if(cache->reserve(nameKey)) {
            std::shared_ptr<Image> img = cache->get(nameKey);
            if(img->type() == STATIC) {
                auto imgStatic = dynamic_cast<ImageStatic *>(img.get());
                imgStatic->setEditedImage(std::unique_ptr<const QImage>(
                            ImageLib::scaled(imgStatic->getImage(), size, 1)));
                cache->release(nameKey);
                displayImage(img.get());
            } else {
                cache->release(nameKey);
                mw->showMessage("Editing gifs/video is unsupported.");
            }
        } else {
            qDebug() << "Core::resize() - could not lock cache object.";
        }
    }
}

// glowing_brain.jpg
void Core::flipH() {
    if(state.hasActiveImage) {
        QString nameKey = dirManager->fileNameAt(state.currentIndex);
        if(cache->reserve(nameKey)) {
            std::shared_ptr<Image> img = cache->get(nameKey);
            if(img && img->type() == STATIC) {
                auto imgStatic = dynamic_cast<ImageStatic *>(img.get());
                imgStatic->setEditedImage(std::unique_ptr<const QImage>(
                            ImageLib::flippedH(imgStatic->getImage())));
                cache->release(nameKey);
                displayImage(img.get());
            } else {
                cache->release(nameKey);
                mw->showMessage("Editing gifs/video is unsupported.");
            }
        } else {
            qDebug() << "Core::flipH() - could not lock cache object.";
        }
    }
}

void Core::flipV() {
    if(state.hasActiveImage) {
        QString nameKey = dirManager->fileNameAt(state.currentIndex);
        if(cache->reserve(nameKey)) {
            std::shared_ptr<Image> img = cache->get(nameKey);
            if(img && img->type() == STATIC) {
                auto imgStatic = dynamic_cast<ImageStatic *>(img.get());
                imgStatic->setEditedImage(std::unique_ptr<const QImage>(
                            ImageLib::flippedV(imgStatic->getImage())));
                cache->release(nameKey);
                displayImage(img.get());
            } else {
                cache->release(nameKey);
                mw->showMessage("Editing gifs/video is unsupported.");
            }
        } else {
            qDebug() << "Core::flipV() - could not lock cache object.";
        }
    }
}

// TODO: simplify. too much copypasted code
void Core::crop(QRect rect) {
    if(state.hasActiveImage) {
        QString nameKey = dirManager->fileNameAt(state.currentIndex);
        if(cache->reserve(nameKey)) {
            std::shared_ptr<Image> img = cache->get(nameKey);
            if(img->type() == STATIC) {
                auto imgStatic = dynamic_cast<ImageStatic *>(img.get());
                if(!imgStatic->setEditedImage(std::unique_ptr<const QImage>(
                            ImageLib::cropped(imgStatic->getImage(), rect))))
                {
                    mw->showMessage("Could not crop image: incorrect size / position");
                }
                cache->release(nameKey);
                displayImage(img.get());
            } else {
                cache->release(nameKey);
                mw->showMessage("Editing gifs/video is unsupported.");
            }
        } else {
            qDebug() << "Core::crop() - could not lock cache object.";
        }
    }
}

void Core::rotateByDegrees(int degrees) {
    if(state.hasActiveImage) {
        QString nameKey = dirManager->fileNameAt(state.currentIndex);
        if(cache->reserve(nameKey)) {
            std::shared_ptr<Image> img = cache->get(nameKey);
            if(img && img->type() == STATIC) {
                auto imgStatic = dynamic_cast<ImageStatic *>(img.get());
                imgStatic->setEditedImage(std::unique_ptr<const QImage>(
                            ImageLib::rotated(imgStatic->getImage(), degrees)));
                cache->release(nameKey);
                displayImage(img.get());
            } else {
                cache->release(nameKey);
                mw->showMessage("Editing gifs/video is unsupported.");
            }
        } else {
            qDebug() << "Core::rotateByDegrees() - could not lock cache object.";
        }
    }
}

// TODO: simplify. too much copypasted code
void Core::discardEdits() {
    if(state.hasActiveImage) {
        QString nameKey = dirManager->fileNameAt(state.currentIndex);
        if(cache->reserve(nameKey)) {
            std::shared_ptr<Image> img = cache->get(nameKey);
            if(img->type() == STATIC) {
                auto imgStatic = dynamic_cast<ImageStatic *>(img.get());
                bool ok = imgStatic->discardEditedImage();
                cache->release(nameKey);
                if(ok)
                    displayImage(img.get());
            } else {
                cache->release(nameKey);
            }
        } else {
            qDebug() << "Core::discardEdits() - could not lock cache object.";
        }
    }
    mw->hideSaveOverlay();
}

// TODO: simplify. too much copypasted code
// also move saving logic away from Image container itself
void Core::saveImageToDisk() {
    if(state.hasActiveImage)
        saveImageToDisk(dirManager->filePathAt(state.currentIndex));
}

void Core::saveImageToDisk(QString filePath) {
    if(state.hasActiveImage) {
        QString nameKey = dirManager->fileNameAt(state.currentIndex);
        if(cache->reserve(nameKey)) {
            std::shared_ptr<Image> img = cache->get(nameKey);
            if(img->save(filePath))
                mw->showMessage("File saved.");
            else
                mw->showMessage("Something happened.");
            cache->release(nameKey);
        } else {
            qDebug() << "Core::saveImageToDisk() - could not lock cache object.";
        }
    }
    mw->hideSaveOverlay();
}

void Core::runScript(const QString &scriptName) {
    QString nameKey = dirManager->fileNameAt(state.currentIndex);
    scriptManager->runScript(scriptName, cache->get(nameKey));
}

void Core::scalingRequest(QSize size) {
    if(state.hasActiveImage && !state.isWaitingForLoader) {
        std::shared_ptr<Image> forScale = cache->get(dirManager->fileNameAt(state.currentIndex));
        if(forScale) {
            QString path = dirManager->filePathAt(state.currentIndex);
            scaler->requestScaled(ScalerRequest(forScale.get(), size, path));
        }
    }
}

// TODO: don't use connect? otherwise there is no point using unique_ptr
void Core::onScalingFinished(QPixmap *scaled, ScalerRequest req) {
    if(state.hasActiveImage /* TODO: a better fix > */ && dirManager->filePathAt(state.currentIndex) == req.string) {
        mw->onScalingFinished(std::unique_ptr<QPixmap>(scaled));
    } else {
        delete scaled;
    }
}

void Core::forwardThumbnail(std::shared_ptr<Thumbnail> thumbnail) {
    int index = dirManager->indexOf(thumbnail->name());
    if(index >= 0) {
        mw->onThumbnailReady(index, thumbnail);
    }
}

void Core::trimCache() {
    QList<QString> list;
    list << dirManager->fileNameAt(state.currentIndex - 1);
    list << dirManager->fileNameAt(state.currentIndex);
    list << dirManager->fileNameAt(state.currentIndex + 1);
    cache->trimTo(list);
}

void Core::clearCache() {
    cache->clear();
}

// reset state; clear cache; etc
void Core::reset() {
    state.hasActiveImage = false;
    state.isWaitingForLoader = false;
    state.currentIndex = 0;
    state.displayingFileName = "";
    //viewerWidget->unset();
    this->clearCache();
}

bool Core::setDirectory(QString newPath) {
    if(!dirManager->hasImages() || dirManager->currentDirectoryPath() != newPath) {
        this->reset();
        settings->setLastDirectory(newPath);
        dirManager->setDirectory(newPath);
        mw->setDirectoryPath(newPath);
        mw->populateThumbnailViews(dirManager->fileCount());
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
        preload(1);
    } else {
        // we got an empty directory; show an error
        mw->showMessage("Directory does not contain supported files.");
    }
}

void Core::loadImage(QString path, bool blocking) {
    DocumentInfo *info = new DocumentInfo(path);
    // new directory
    QElapsedTimer t;
    t.start();
    setDirectory(info->directoryPath());
    state.currentIndex = dirManager->indexOf(info->fileName());
    onLoadStarted();
    QString nameKey = dirManager->fileNameAt(state.currentIndex);
    // First check if image is already cached. If it is, just display it.
    if(cache->contains(nameKey))
        displayImage(cache->get(nameKey).get());
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
            displayImage(cache->get(nameKey).get());
        else
            loader->loadExclusive(dirManager->filePathAt(state.currentIndex));
        preload(index - 1);
        preload(index + 1);
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
            displayImage(cache->get(nameKey).get());
        else
            loader->loadExclusive(dirManager->filePathAt(state.currentIndex));
        return true;
    }
    return false;
}

void Core::nextImage() {
    if(dirManager->hasImages()) {
        int index = state.currentIndex + 1;
        if(index >= dirManager->fileCount()) {
            if(infiniteScrolling) {
                index = 0;
            } else {
                if(!state.isWaitingForLoader)
                    mw->showMessageDirectoryEnd();
                return;
            }
        }
        state.currentIndex = index;
        onLoadStarted();
        QString nameKey = dirManager->fileNameAt(state.currentIndex);
        // First check if image is already cached. If it is, just display it.
        if(cache->contains(nameKey))
            displayImage(cache->get(nameKey).get());
        else
            loader->loadExclusive(dirManager->filePathAt(state.currentIndex));
        preload(index + 1);
    }
}

void Core::prevImage() {
    if(dirManager->hasImages()) {
        int index = state.currentIndex - 1;
        if(index < 0) {
            if(infiniteScrolling) {
                index = dirManager->fileCount() - 1;
            }
            else {
                if(!state.isWaitingForLoader)
                    mw->showMessageDirectoryStart();
                return;
            }
        }
        state.currentIndex = index;
        onLoadStarted();
        QString nameKey = dirManager->fileNameAt(state.currentIndex);
        // First check if image is already cached. If it is, just display it.
        if(cache->contains(nameKey))
            displayImage(cache->get(nameKey).get());
        else
            loader->loadExclusive(dirManager->filePathAt(state.currentIndex));
        preload(index - 1);
    }
}

void Core::jumpToFirst() {
    if(dirManager->hasImages()) {
        this->loadByIndex(0);
        mw->showMessageDirectoryStart();
    }
}

void Core::jumpToLast() {
    if(dirManager->hasImages()) {
        this->loadByIndex(dirManager->fileCount() - 1);
        mw->showMessageDirectoryEnd();
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

void Core::onLoadFinished(std::shared_ptr<Image> img) {
    int index = dirManager->indexOf(img->name());
    bool isRelevant = !(index < state.currentIndex - 1 || index > state.currentIndex + 1);
    QString nameKey = dirManager->fileNameAt(index);
    if(isRelevant) {
        if(!cache->insert(nameKey, img)) {
            img = cache->get(nameKey);
        }
    }
    if(index == state.currentIndex) {
        // display
        displayImage(img.get());
        // preload nearest
        preload(state.currentIndex - 1);
        preload(state.currentIndex + 1);
    }
}

void Core::onLoadFailed(QString path) {
    mw->showMessage("Load failed: " + path);
    if(path == dirManager->filePathAt(state.currentIndex))
        mw->closeImage();
}

void Core::displayImage(Image *img) {
    loadingTimer->stop();
    state.isWaitingForLoader = false;
    state.hasActiveImage = true;
    if(img) {  // && img->name() != state.displayingFileName) {
        DocumentType type = img->type();
        if(type == STATIC) {
            mw->showImage(img->getPixmap());
        } else if(type == ANIMATED) {
            auto animated = dynamic_cast<ImageAnimated *>(img);
            mw->showAnimation(animated->getMovie());
        } else if(type == VIDEO) {
            auto video = dynamic_cast<Video *>(img);
            // workaround for mpv. If we play video while mainwindow is hidden we get black screen.
            // affects only initial startup (e.g. we open webm from file manager)
            showGui();
            mw->showVideo(video->getClip());
        }
        state.displayingFileName = img->name();
        img->isEdited()?mw->showSaveOverlay():mw->hideSaveOverlay();
        emit imageIndexChanged(state.currentIndex);
        updateInfoString();
    } else {
        mw->showMessage("Error: could not load image.");
    }
}

// TODO: this looks ugly
void Core::updateInfoString() {
    QString infoString = "";
    if(dirManager->fileCount()) {
        infoString.append("[ " +
                          QString::number(state.currentIndex + 1) +
                          "/" +
                          QString::number(dirManager->fileCount()) +
                          " ]   ");
    }
    if(!state.isWaitingForLoader) {
        std::shared_ptr<Image> img = cache->get(dirManager->fileNameAt(state.currentIndex));
        QString name, fullName = img->name();
        if(fullName.size()>95) {
            name = fullName.left(95);
            name.append(" (...) ");
            name.append(fullName.right(12));
        } else {
            name = fullName;
        }
        infoString.append(name + "  (");
        if(img->width()) {
            infoString.append(QString::number(img->width()) +
                              " x " +
                              QString::number(img->height()) +
                              "  ");
        }
        infoString.append(QString::number(img->fileSize()) + " KB)");
    }
    mw->setInfoString(infoString);
}
