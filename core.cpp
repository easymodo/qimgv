/*
 * This is sort-of a main controller of application.
 * It creates and initializes all components, then sets up gui and actions.
 * Most of communication between components go through here.
 *
 */

#include "core.h"

Core::Core() : QObject(), infiniteScrolling(false) {
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
    model.reset(new DirectoryModel());
    presenter.setModel(model);
}

void Core::connectComponents() {
    presenter.connectView(mw->getFolderView());
    presenter.connectView(mw->getThumbnailPanel());

    connect(&model->loader, SIGNAL(loadFinished(std::shared_ptr<Image>)),
            this, SLOT(onLoadFinished(std::shared_ptr<Image>)));
    connect(&model->loader, SIGNAL(loadFailed(QString)),
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
    connect(this, SIGNAL(currentIndexChanged(int)), mw, SLOT(setupSidePanelData()));

    // scaling
    connect(mw, SIGNAL(scalingRequested(QSize)),
            this, SLOT(scalingRequest(QSize)));
    connect(model->scaler, SIGNAL(scalingFinished(QPixmap*,ScalerRequest)),
            this, SLOT(onScalingFinished(QPixmap*,ScalerRequest)));

    // filesystem changes
    connect(model.get(), SIGNAL(fileRemoved(QString, int)), this, SLOT(onFileRemoved(QString, int)));
    connect(model.get(), SIGNAL(fileAdded(QString)), this, SLOT(onFileAdded(QString)));
    connect(model.get(), SIGNAL(fileModified(QString)), this, SLOT(onFileModified(QString)));
    connect(model.get(), SIGNAL(fileRenamed(QString, QString)), this, SLOT(onFileRenamed(QString, QString)));
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
    connect(actionManager, SIGNAL(reloadImage()), this, SLOT(reloadImage()));
    connect(actionManager, SIGNAL(copyFileClipboard()), this, SLOT(copyFileClipboard()));
    connect(actionManager, SIGNAL(copyPathClipboard()), this, SLOT(copyPathClipboard()));
    connect(actionManager, SIGNAL(renameFile()), this, SLOT(renameRequested()));
    connect(actionManager, SIGNAL(contextMenu()), mw, SLOT(showContextMenu()));
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
    model->thumbnailer->clearTasks();
    model->loader.clearTasks();
}

void Core::close() {
    mw->close();
    closeBackgroundTasks();
}

void Core::removeFilePermanent() {
    if(state.hasActiveImage)
        removeFilePermanent(model->currentFileName);
}

void Core::removeFilePermanent(QString fileName) {
    removeFile(fileName, false);
}

void Core::moveToTrash() {
    if(state.hasActiveImage)
        moveToTrash(model->currentFileName);
}

void Core::moveToTrash(QString fileName) {
    removeFile(fileName, true);
}

void Core::reloadImage() {
    reloadImage(model->currentFileName);
}

void Core::reloadImage(QString fileName) {
    if(!model->contains(fileName))
        return;
    model->cache.remove(fileName);
    if(model->currentFileName == fileName)
        loadByPathBlocking(model->fullFilePath(model->currentFileName));
}

// TODO: also copy selection from folder view?
void Core::copyFileClipboard() {
    if(model->currentFileName.isEmpty())
        return;
    QMimeData* mimeData = new QMimeData();
    mimeData->setUrls({QUrl::fromLocalFile(model->fullFilePath(model->currentFileName))});

    // gnome being special
    // doesn't seem to work
    //QByteArray gnomeFormat = QByteArray("copy\n").append(QUrl::fromLocalFile(path).toEncoded());
    //mimeData->setData("x-special/gnome-copied-files", gnomeFormat);

    QApplication::clipboard()->setMimeData(mimeData);
    mw->showMessage("File copied");
}

void Core::copyPathClipboard() {
    if(model->currentFileName.isEmpty())
        return;
    QApplication::clipboard()->setText(model->fullFilePath(model->currentFileName));
    mw->showMessage("Path copied");
}

void Core::renameRequested() {
    renameCurrentFile("new.png");
}

void Core::renameCurrentFile(QString newName) {
    QString newPath = model->fullFilePath(newName);
    QString currentPath = model->fullFilePath(model->currentFileName);
    bool exists = model->contains(newName);
    QFile replaceMe(newPath);
    // move existing file so we can revert if something fails
    if(replaceMe.exists()) {
        if(!replaceMe.rename(newPath + "__tmp")) {
            mw->showError("Could not replace file");
            return;
        }
    }
    // do the renaming
    QFile file(currentPath);
    if(file.exists() && file.rename(newPath)) {
        // remove tmp file on success
        if(exists)
            replaceMe.remove();
        // at this point we will get a dirwatcher rename event
        // and the new file will be opened
    } else {
        mw->showError("Could not rename file");
        // revert tmp file on fail
        if(exists)
            replaceMe.rename(newPath);
    }
}

// removes file at specified index within current directory
void Core::removeFile(QString fileName, bool trash) {
    if(model->removeFile(fileName, trash)) {
        QString msg = trash?"Moved to trash: ":"File removed: ";
        mw->showMessage(msg + fileName);
    }
}

void Core::onFileRemoved(QString fileName, int index) {
    model->cache.remove(fileName);
//    mw->removeThumbnail(index);
    // removing current file. try switching to another
    if(model->currentFileName == fileName) {
        if(!model->itemCount()) {
            mw->closeImage();
        } else {
            if(!loadByIndex(index))
                loadByIndex(--index);
        }
    }
}

void Core::onFileRenamed(QString from, QString to) {
    model->cache.remove(from);
    if(model->currentFileName == from) {
        model->cache.clear();
        this->loadByPathBlocking(model->fullFilePath(to));
    }
    //mw->removeThumbnail(oldIndex);
    //mw->addThumbnail(newIndex);
}

void Core::onFileAdded(QString fileName) {
    //int index = model->indexOf(fileName);
    //mw->addThumbnail(index);

    emit currentIndexChanged(model->indexOf(model->currentFileName));
    updateInfoString();
}

void Core::onFileModified(QString fileName) {
    if(model->cache.contains(fileName)) {
        QDateTime modTime = model->lastModified(fileName);
        std::shared_ptr<Image> img = model->cache.get(fileName);
        if(modTime.isValid() && modTime > img->lastModified()) {
            if(fileName == model->currentFileName) {
                mw->showMessage("File changed on disk. Reloading.");
                reloadImage(fileName);
            } else {
                model->cache.remove(fileName);
            }
        }
    }
}

void Core::moveFile(QString destDirectory) {
    model->moveTo(destDirectory, model->currentFileName);
    /*
    QFileInfo file(destDirectory + "/" + model->currentFileName);
    if(file.exists()) {
        mw->showMessage("Error: file already exists.");
        return;
    }
    if(model->copyTo(destDirectory, model->currentFileName)) {
        removeFilePermanent();
        mw->showMessage("File moved to: " + destDirectory);
    } else {
        mw->showMessage("Error moving file to: " + destDirectory);
        qDebug() << "Error moving file to: " << destDirectory;
    }
    */
}

void Core::copyFile(QString destDirectory) {
    model->copyTo(destDirectory, model->currentFileName);
    /*QFileInfo file(destDirectory + "/" + model->currentFileName);
    if(file.exists()) {
        if(file.isFile())
            mw->showMessage("Error: file already exists.");
        return;
    }
    if(!model->copyTo(destDirectory, model->currentFileName)) {
        mw->showMessage("Error copying file to: " + destDirectory);
        qDebug() << "Error copying file to: " << destDirectory;
    } else {
        mw->showMessage("File copied to: " + destDirectory);
    }
    */
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
        mw->showSaveDialog(model->fullFilePath(model->currentFileName));
    }
}

void Core::showResizeDialog() {
    if(state.hasActiveImage) {
        mw->showResizeDialog(model->cache.get(model->currentFileName)->size());
    }
}

// TODO: simplify. too much copypasted code
// use default bilinear for now
void Core::resize(QSize size) {
    if(state.hasActiveImage) {
        if(model->cache.reserve(model->currentFileName)) {
            std::shared_ptr<Image> img = model->cache.get(model->currentFileName);
            if(img->type() == STATIC) {
                auto imgStatic = dynamic_cast<ImageStatic *>(img.get());
                imgStatic->setEditedImage(std::unique_ptr<const QImage>(
                            ImageLib::scaled(imgStatic->getImage(), size, 1)));
                model->cache.release(model->currentFileName);
                displayImage(img.get());
            } else {
                model->cache.release(model->currentFileName);
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
        if(model->cache.reserve(model->currentFileName)) {
            std::shared_ptr<Image> img = model->cache.get(model->currentFileName);
            if(img && img->type() == STATIC) {
                auto imgStatic = dynamic_cast<ImageStatic *>(img.get());
                imgStatic->setEditedImage(std::unique_ptr<const QImage>(
                            ImageLib::flippedH(imgStatic->getImage())));
                model->cache.release(model->currentFileName);
                displayImage(img.get());
            } else {
                model->cache.release(model->currentFileName);
                mw->showMessage("Editing gifs/video is unsupported.");
            }
        } else {
            qDebug() << "Core::flipH() - could not lock cache object.";
        }
    }
}

void Core::flipV() {
    if(state.hasActiveImage) {
        if(model->cache.reserve(model->currentFileName)) {
            std::shared_ptr<Image> img = model->cache.get(model->currentFileName);
            if(img && img->type() == STATIC) {
                auto imgStatic = dynamic_cast<ImageStatic *>(img.get());
                imgStatic->setEditedImage(std::unique_ptr<const QImage>(
                            ImageLib::flippedV(imgStatic->getImage())));
                model->cache.release(model->currentFileName);
                displayImage(img.get());
            } else {
                model->cache.release(model->currentFileName);
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
        if(model->cache.reserve(model->currentFileName)) {
            std::shared_ptr<Image> img = model->cache.get(model->currentFileName);
            if(img->type() == STATIC) {
                auto imgStatic = dynamic_cast<ImageStatic *>(img.get());
                if(!imgStatic->setEditedImage(std::unique_ptr<const QImage>(
                            ImageLib::cropped(imgStatic->getImage(), rect))))
                {
                    mw->showMessage("Could not crop image: incorrect size / position");
                }
                model->cache.release(model->currentFileName);
                displayImage(img.get());
            } else {
                model->cache.release(model->currentFileName);
                mw->showMessage("Editing gifs/video is unsupported.");
            }
        } else {
            qDebug() << "Core::crop() - could not lock cache object.";
        }
    }
}

void Core::rotateByDegrees(int degrees) {
    if(state.hasActiveImage) {
        if(model->cache.reserve(model->currentFileName)) {
            std::shared_ptr<Image> img = model->cache.get(model->currentFileName);
            if(img && img->type() == STATIC) {
                auto imgStatic = dynamic_cast<ImageStatic *>(img.get());
                imgStatic->setEditedImage(std::unique_ptr<const QImage>(
                            ImageLib::rotated(imgStatic->getImage(), degrees)));
                model->cache.release(model->currentFileName);
                displayImage(img.get());
            } else {
                model->cache.release(model->currentFileName);
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
        if(model->cache.reserve(model->currentFileName)) {
            std::shared_ptr<Image> img = model->cache.get(model->currentFileName);
            if(img->type() == STATIC) {
                auto imgStatic = dynamic_cast<ImageStatic *>(img.get());
                bool ok = imgStatic->discardEditedImage();
                model->cache.release(model->currentFileName);
                if(ok)
                    displayImage(img.get());
            } else {
                model->cache.release(model->currentFileName);
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
        saveImageToDisk(model->fullFilePath(model->currentFileName));
}

void Core::saveImageToDisk(QString filePath) {
    if(state.hasActiveImage) {
        if(model->cache.reserve(model->currentFileName)) {
            std::shared_ptr<Image> img = model->cache.get(model->currentFileName);
            if(img->save(filePath))
                mw->showMessageSuccess("File saved.");
            else
                mw->showError("Could not save file.");
            model->cache.release(model->currentFileName);
        } else {
            qDebug() << "Core::saveImageToDisk() - could not lock cache object.";
        }
    }
    mw->hideSaveOverlay();
}

void Core::runScript(const QString &scriptName) {
    scriptManager->runScript(scriptName, model->cache.get(model->currentFileName));
}

void Core::scalingRequest(QSize size) {
    if(state.hasActiveImage && !state.isWaitingForLoader) {
        std::shared_ptr<Image> forScale = model->cache.get(model->currentFileName);
        if(forScale) {
            QString path = model->absolutePath() + "/" + model->currentFileName;
            model->scaler->requestScaled(ScalerRequest(forScale.get(), size, path));
        }
    }
}

// TODO: don't use connect? otherwise there is no point using unique_ptr
void Core::onScalingFinished(QPixmap *scaled, ScalerRequest req) {
    if(state.hasActiveImage /* TODO: a better fix > */ && req.string == model->fullFilePath(model->currentFileName)) {
        mw->onScalingFinished(std::unique_ptr<QPixmap>(scaled));
    } else {
        delete scaled;
    }
}

void Core::trimCache() {
    QList<QString> list;
    list << model->prevOf(model->currentFileName);
    list << model->currentFileName;
    list << model->nextOf(model->currentFileName);
    model->cache.trimTo(list);
}

void Core::clearCache() {
    model->cache.clear();
}

// reset state; clear cache; etc
void Core::reset() {
    state.hasActiveImage = false;
    state.isWaitingForLoader = false;
    model->currentFileName = "";
    //viewerWidget->unset();
    this->clearCache();
}

bool Core::setDirectory(QString newPath) {
    if(model->isEmpty() || model->absolutePath() != newPath) {
        this->reset();
        settings->setLastDirectory(newPath);
        model->setDirectory(newPath);
        mw->setDirectoryPath(newPath);
        return true;
    }
    return false;
}

void Core::loadDirectory(QString path) {
    this->reset();
    // new directory
    setDirectory(path);
    if(!model->isEmpty()) {
        // open the first image
        this->loadByIndex(0);
        preload(model->fileNameAt(1));
    } else {
        // we got an empty directory; show an error
        mw->showMessage("Directory does not contain supported files.");
    }
}

void Core::loadImage(QString path, bool blocking) {
    QFileInfo info(path);
    // new directory
    setDirectory(info.absolutePath());
    model->currentFileName = info.fileName();
    onLoadStarted();
    // First check if image is already cached. If it is, just display it.
    if(model->cache.contains(info.fileName()))
        displayImage(model->cache.get(info.fileName()).get());
    else if(blocking)
        model->loader.loadBlocking(path);
    else
        model->loader.loadExclusive(path);
}

void Core::loadByPath(QString path, bool blocking) {
    if(path.startsWith("file://", Qt::CaseInsensitive)) {
        path.remove(0, 7);
    }
    /*if(model->dirManager.isImage(path)) {
        loadImage(path, blocking);
    } else if(model->dirManager.isDirectory(path)) {
        loadDirectory(path);
    } else {
        mw->showMessage("File does not exist or is not supported.");
        qDebug() << "Could not open path: " << path;
    }
    */
    loadImage(path, blocking);
}

void Core::loadByPath(QString filePath) {
    loadByPath(filePath, false);
}

void Core::loadByPathBlocking(QString filePath) {
    loadByPath(filePath, true);
}

bool Core::loadByIndex(int index) {
    if(index >= 0 && index < model->itemCount()) {
        model->currentFileName = model->fileNameAt(index);
        onLoadStarted();
        // First check if image is already cached. If it is, just display it.
        // force reload??
        if(model->cache.contains(model->currentFileName)) {
            displayImage(model->cache.get(model->currentFileName).get());
        } else {
            model->loader.loadExclusive(model->fullFilePath(model->currentFileName));
        }
        preload(model->prevOf(model->currentFileName));
        preload(model->nextOf(model->currentFileName));
        return true;
    }
    return false;
}

void Core::nextImage() {
    if(model->isEmpty())
        return;
    QString next = model->nextOf(model->currentFileName);
    if(next.isEmpty()) {
        if(infiniteScrolling) {
            next = model->first();
        } else {
            if(!state.isWaitingForLoader)
                mw->showMessageDirectoryEnd();
            return;
        }
    }
    model->currentFileName = next;
    onLoadStarted();
    if(model->cache.contains(model->currentFileName))
        displayImage(model->cache.get(model->currentFileName).get());
    else
        model->loader.loadExclusive(model->fullFilePath(model->currentFileName));
    preload(model->nextOf(model->currentFileName));
}

void Core::prevImage() {
    if(model->isEmpty())
        return;
    QString next = model->prevOf(model->currentFileName);
    if(next.isEmpty()) {
        if(infiniteScrolling) {
            next = model->last();
        } else {
            if(!state.isWaitingForLoader)
                mw->showMessageDirectoryStart();
            return;
        }
    }
    model->currentFileName = next;
    onLoadStarted();
    if(model->cache.contains(model->currentFileName))
        displayImage(model->cache.get(model->currentFileName).get());
    else
        model->loader.loadExclusive(model->fullFilePath(model->currentFileName));
    preload(model->prevOf(model->currentFileName));
}

void Core::jumpToFirst() {
    if(!model->isEmpty()) {
        this->loadByIndex(0);
        mw->showMessageDirectoryStart();
    }
}

void Core::jumpToLast() {
    if(!model->isEmpty()) {
        this->loadByIndex(model->itemCount() - 1);
        mw->showMessageDirectoryEnd();
    }
}

void Core::preload(QString fileName) {
    if(settings->usePreloader() && model->contains(fileName) && !model->cache.contains(fileName))
        model->loader.load(model->fullFilePath(fileName));
}

void Core::onLoadStarted() {
    state.isWaitingForLoader = true;
    trimCache();
}

void Core::onLoadFinished(std::shared_ptr<Image> img) {
    if(img) {
        bool isRelevant = (img->name() == model->currentFileName)
                || (img->name() == model->prevOf(model->currentFileName))
                || (img->name() == model->nextOf(model->currentFileName));
        if(isRelevant) {
            if(!model->cache.insert(img)) {
                img = model->cache.get(img->name());
            }
        }
        if(img->name() == model->currentFileName) {
            // display
            displayImage(img.get());
            // preload nearest
            preload(model->prevOf(model->currentFileName));
            preload(model->nextOf(model->currentFileName));
        }
    }
}

void Core::onLoadFailed(QString path) {
    mw->showMessage("Load failed: " + path);
    QString currentPath = model->fullFilePath(model->currentFileName);
    if(path == currentPath)
        mw->closeImage();
}

void Core::displayImage(Image *img) {
    state.isWaitingForLoader = false;
    state.hasActiveImage = true;
    if(img) {
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
        img->isEdited()?mw->showSaveOverlay():mw->hideSaveOverlay();

        int index = model->indexOf(model->currentFileName);
        emit currentIndexChanged(index);
        presenter.setCurrentIndex(index);

        updateInfoString();
    } else {
        mw->showMessage("Error: could not load image.");
    }
}

void Core::updateInfoString() {
    QSize imageSize(0,0);
    int fileSize = 0;
    std::shared_ptr<Image> img = model->cache.get(model->currentFileName);
    if(img) {
        imageSize = img->size();
        fileSize  = img->fileSize();
    }
    mw->setCurrentInfo(model->indexOf(model->currentFileName),
                       model->itemCount(),
                       model->currentFileName,
                       imageSize,
                       fileSize);
}
