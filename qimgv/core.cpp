/*
 * This is sort-of a main controller of application.
 * It creates and initializes all components, then sets up gui and actions.
 * Most of communication between components go through here.
 *
 */

#include "core.h"

Core::Core() : QObject(), infiniteScrolling(false), mDrag(nullptr) {
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
    if(settings->firstRun())
        onFirstRun();
    else if(appVersion > lastVersion)
        onUpdate();
}

void Core::readSettings() {
    infiniteScrolling = settings->infiniteScrolling();
    if(settings->shuffleEnabled())
        syncRandomizer();
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

void Core::attachModel(DirectoryModel *_model) {
    model.reset(_model);
    presenter.setModel(model);
    if(settings->shuffleEnabled())
        syncRandomizer();
}

void Core::initComponents() {
    attachModel(new DirectoryModel());
}

void Core::connectComponents() {
    presenter.connectView(mw->getFolderView());
    presenter.connectView(mw->getThumbnailPanel());

    /*connect(&model->loader, SIGNAL(loadFinished(std::shared_ptr<Image>)),
            this, SLOT(onLoadFinished(std::shared_ptr<Image>)));
    connect(&model->loader, SIGNAL(loadFailed(QString)),
            this, SLOT(onLoadFailed(QString)));
    */
    connect(mw, SIGNAL(opened(QString)), this, SLOT(loadPath(QString)));
    connect(mw, SIGNAL(copyRequested(QString)), this, SLOT(copyFile(QString)));
    connect(mw, SIGNAL(moveRequested(QString)), this, SLOT(moveFile(QString)));
    connect(mw, SIGNAL(resizeRequested(QSize)), this, SLOT(resize(QSize)));
    connect(mw, SIGNAL(cropRequested(QRect)), this, SLOT(crop(QRect)));
    connect(mw, SIGNAL(saveAsClicked()), this, SLOT(requestSavePath()));
    connect(mw, SIGNAL(saveRequested()), this, SLOT(saveImageToDisk()));
    connect(mw, SIGNAL(saveRequested(QString)), this, SLOT(saveImageToDisk(QString)));
    connect(mw, SIGNAL(discardEditsRequested()), this, SLOT(discardEdits()));
    connect(mw, SIGNAL(sortingSelected(SortingMode)), this, SLOT(sortBy(SortingMode)));
    connect(mw, SIGNAL(draggedOut()), this, SLOT(onDragOut()));
    connect(mw, SIGNAL(droppedIn(const QMimeData*, QObject*)), this, SLOT(onDropIn(const QMimeData*, QObject*)));
    connect(mw, &MainWindow::renameRequested, this, &Core::renameCurrentFile);

    // scaling
    connect(mw, SIGNAL(scalingRequested(QSize, ScalingFilter)),
            this, SLOT(scalingRequest(QSize, ScalingFilter)));
    connect(model->scaler, SIGNAL(scalingFinished(QPixmap*,ScalerRequest)),
            this, SLOT(onScalingFinished(QPixmap*,ScalerRequest)));

    // filesystem changes
    connect(model.get(), SIGNAL(fileRemoved(QString, int)), this, SLOT(onFileRemoved(QString, int)));
    connect(model.get(), SIGNAL(fileAdded(QString)), this, SLOT(onFileAdded(QString)));
    connect(model.get(), SIGNAL(fileModified(QString)), this, SLOT(onFileModified(QString)));
    connect(model.get(), SIGNAL(fileRenamed(QString, int, QString, int)), this, SLOT(onFileRenamed(QString, int, QString, int)));

    connect(model.get(), SIGNAL(itemReady(std::shared_ptr<Image>)), this, SLOT(onModelItemReady(std::shared_ptr<Image>)));
    connect(model.get(), SIGNAL(itemUpdated(std::shared_ptr<Image>)), this, SLOT(onModelItemUpdated(std::shared_ptr<Image>)));
    connect(model.get(), SIGNAL(indexChanged(int, int)), this, SLOT(updateInfoString()));
    connect(model.get(), SIGNAL(sortingChanged()), this, SLOT(updateInfoString()));
    connect(model.get(), SIGNAL(loaded(QString)), this, SLOT(onModelLoaded()));
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
    connect(actionManager, SIGNAL(folderView()), mw, SLOT(enableFolderView()));
    connect(actionManager, SIGNAL(documentView()), mw, SIGNAL(enableDocumentView()));
    connect(actionManager, SIGNAL(toggleFolderView()), mw, SLOT(toggleFolderView()));
    connect(actionManager, SIGNAL(reloadImage()), this, SLOT(reloadImage()));
    connect(actionManager, SIGNAL(copyFileClipboard()), this, SLOT(copyFileClipboard()));
    connect(actionManager, SIGNAL(copyPathClipboard()), this, SLOT(copyPathClipboard()));
    connect(actionManager, SIGNAL(renameFile()), this, SLOT(showRenameDialog()));
    connect(actionManager, SIGNAL(contextMenu()), mw, SLOT(showContextMenu()));
    connect(actionManager, SIGNAL(toggleTransparencyGrid()), mw, SIGNAL(toggleTransparencyGrid()));
    connect(actionManager, SIGNAL(sortByName()), this, SLOT(sortByName()));
    connect(actionManager, SIGNAL(sortByTime()), this, SLOT(sortByTime()));
    connect(actionManager, SIGNAL(sortBySize()), this, SLOT(sortBySize()));
    connect(actionManager, SIGNAL(toggleImageInfo()), mw, SLOT(toggleImageInfoOverlay()));
    connect(actionManager, SIGNAL(toggleShuffle()), this, SLOT(toggleShuffle()));
    connect(actionManager, SIGNAL(toggleScalingFilter()), mw, SLOT(toggleScalingFilter()));
}

void Core::onUpdate() {
    QVersionNumber lastVer = settings->lastVersion();
    actionManager->resetDefaultsFromVersion(lastVer);
    actionManager->saveShortcuts();
    qDebug() << "Updated: " << settings->lastVersion().toString() << ">" << appVersion.toString();
    // TODO: finish changelogs
    //if(settings->showChangelogs())
    //    mw->showChangelogWindow();
    mw->showMessage("Updated: " + settings->lastVersion().toString() + " > " + appVersion.toString(), 4000);
    settings->setLastVersion(appVersion);
}

void Core::onFirstRun() {
    //mw->showSomeSortOfWelcomeScreen();
    mw->showMessage("Welcome to qimgv version " + appVersion.toString() + "!", 4000);
    settings->setFirstRun(false);
    settings->setLastVersion(appVersion);
}

void Core::toggleShuffle() {
    if(settings->shuffleEnabled()) {
        settings->setShuffleEnabled(false);
        mw->showMessage("Shuffle Disabled");
    } else {
        settings->setShuffleEnabled(true);
        syncRandomizer();
        mw->showMessage("Shuffle Enabled");
    }
}

void Core::syncRandomizer() {
    if(model) {
        randomizer.setCount(model->itemCount());
        randomizer.shuffle();
        randomizer.setCurrent(model->currentIndex());
    }
}

void Core::onModelLoaded() {
    if(settings->shuffleEnabled())
        syncRandomizer();
}

void Core::rotateLeft() {
    rotateByDegrees(-90);
}

void Core::rotateRight() {
    rotateByDegrees(90);
}

void Core::close() {
    mw->close();
}

void Core::removeFilePermanent() {
    if(state.hasActiveImage)
        removeFilePermanent(model->currentFileName());
}

void Core::removeFilePermanent(QString fileName) {
    removeFile(fileName, false);
}

void Core::moveToTrash() {
    if(state.hasActiveImage)
        moveToTrash(model->currentFileName());
}

void Core::moveToTrash(QString fileName) {
    removeFile(fileName, true);
}

void Core::reloadImage() {
    reloadImage(model->currentFileName());
}

void Core::reloadImage(QString fileName) {
    if(!model->contains(fileName))
        return;
    model->cache.remove(fileName);
    if(model->currentFileName() == fileName)
        loadPath(model->currentFilePath());
}

// TODO: also copy selection from folder view?
void Core::copyFileClipboard() {
    if(!model || model->currentFileName().isEmpty())
        return;

    QMimeData* mimeData = getMimeDataFor(model->getItemAt(model->currentIndex()), TARGET_CLIPBOARD);

    // mimeData->text() should already contain an url
    QByteArray gnomeFormat = QByteArray("copy\n").append(QUrl(mimeData->text()).toEncoded());
    mimeData->setData("x-special/gnome-copied-files", gnomeFormat);
    mimeData->setData("application/x-kde-cutselection", "0");

    QApplication::clipboard()->setMimeData(mimeData);
    mw->showMessage("File copied");
}

void Core::copyPathClipboard() {
    if(model->currentFileName().isEmpty())
        return;
    QApplication::clipboard()->setText(model->currentFilePath());
    mw->showMessage("Path copied");
}

void Core::onDropIn(const QMimeData *mimeData, QObject* source) {
    // ignore self
    if(source == this)
        return;
    // check for our needed mime type, here a file or a list of files
    if(mimeData->hasUrls()) {
        QStringList pathList;
        QList<QUrl> urlList = mimeData->urls();
        // extract the local paths of the files
        for(int i = 0; i < urlList.size() && i < 32; ++i) {
            pathList.append(urlList.at(i).toLocalFile());
        }
        // try to open first file in the list
        loadPath(pathList.first());
    }
}

// drag'n'drop
// drag image out of the program
void Core::onDragOut() {
    if(!model || !model->itemCount())
        return;

    QPoint hotspot(0,0);
    QPixmap pixmap(":/res/icons/app/64.png"); // use some thumbnail here

    QMimeData *mimeData = getMimeDataFor(model->getItemAt(model->currentIndex()), TARGET_DROP);

    mDrag = new QDrag(this);
    mDrag->setMimeData(mimeData);
    mDrag->setPixmap(pixmap);
    mDrag->setHotSpot(hotspot);

    mDrag->exec(Qt::MoveAction | Qt::CopyAction | Qt::LinkAction, Qt::CopyAction);
}

QMimeData *Core::getMimeDataFor(std::shared_ptr<Image> img, MimeDataTarget target) {
    QMimeData* mimeData = new QMimeData();
    if(!img)
        return mimeData;
    QString path = img->path();
    if(img->type() == STATIC) {
        if(img->isEdited()) {
            // TODO: cleanup temp files
            // meanwhile use generic name
            //path = settings->cacheDir() + img->baseName() + ".png";
            path = settings->cacheDir() + "image.png";
            // use faster compression for drag'n'drop
            int pngQuality = (target == TARGET_DROP) ? 80 : 30;
            img->getImage()->save(path, nullptr, pngQuality);
        }
    }
    // !!! using setImageData() while doing drag'n'drop hangs Xorg !!!
    // clipboard only!
    if(img->type() != VIDEO && target == TARGET_CLIPBOARD)
        mimeData->setImageData(*img->getImage().get());
    mimeData->setUrls({QUrl::fromLocalFile(path)});
    return mimeData;
}

void Core::sortBy(SortingMode mode) {
    model->setSortingMode(mode);
}

void Core::renameCurrentFile(QString newName) {
    QString newPath = model->fullPath(newName);
    QString currentPath = model->currentFilePath();
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
    // removing current file. try switching to another
    if(!model->itemCount()) {
        mw->closeImage();
    } else if(model->currentFileName() == fileName) {
        if(!model->setIndexAsync(index))
            model->setIndexAsync(--index);
    }
    updateInfoString();
}

void Core::onFileRenamed(QString from, int indexFrom, QString to, int indexTo) {
    model->cache.remove(from);
    if(model->currentFileName() == from) {
        model->cache.clear(); // ? do it in the model itself
        model->setIndexAsync(indexTo);
    }
}

void Core::onFileAdded(QString fileName) {
    Q_UNUSED(fileName)
    // update file count
    updateInfoString();
}

void Core::onFileModified(QString fileName) {
    if(model->cache.contains(fileName)) {
        QDateTime modTime = model->lastModified(fileName);
        std::shared_ptr<Image> img = model->cache.get(fileName);
        if(modTime.isValid() && modTime > img->lastModified()) {
            if(fileName == model->currentFileName()) {
                mw->showMessage("File changed on disk. Reloading.");
                reloadImage(fileName);
            } else {
                model->cache.remove(fileName);
            }
        }
    }
}

void Core::moveFile(QString destDirectory) {
    model->moveTo(destDirectory, model->currentFileName());
}

void Core::copyFile(QString destDirectory) {
    model->copyTo(destDirectory, model->currentFileName());
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
        mw->showSaveDialog(model->currentFilePath());
    }
}

void Core::showResizeDialog() {
    if(state.hasActiveImage) {
        mw->showResizeDialog(model->cache.get(model->currentFileName())->size());
    }
}

// all editing operations should be done in the main thread
// do an access wrapper with edit function as argument?
void Core::resize(QSize size) {
    if(!state.hasActiveImage)
        return;
    std::shared_ptr<Image> img = model->getItem(model->currentFileName());
    if(img && img->type() == STATIC) {
        auto imgStatic = dynamic_cast<ImageStatic *>(img.get());
        imgStatic->setEditedImage(std::unique_ptr<const QImage>(
                    ImageLib::scaled(imgStatic->getImage(), size, 1)));
        model->updateItem(model->currentFileName(), img);
    } else {
        mw->showMessage("Editing gifs/video is unsupported.");
    }
}

void Core::flipH() {
    if(!state.hasActiveImage)
        return;
    std::shared_ptr<Image> img = model->getItem(model->currentFileName());
    if(img && img->type() == STATIC) {
        auto imgStatic = dynamic_cast<ImageStatic *>(img.get());
        imgStatic->setEditedImage(std::unique_ptr<const QImage>(
                    ImageLib::flippedH(imgStatic->getImage())));
        model->updateItem(model->currentFileName(), img);
    } else {
        mw->showMessage("Editing gifs/video is unsupported.");
    }
}

void Core::flipV() {
    if(!state.hasActiveImage)
        return;
    std::shared_ptr<Image> img = model->getItem(model->currentFileName());
    if(img && img->type() == STATIC) {
        auto imgStatic = dynamic_cast<ImageStatic *>(img.get());
        imgStatic->setEditedImage(std::unique_ptr<const QImage>(
                    ImageLib::flippedV(imgStatic->getImage())));
        model->updateItem(model->currentFileName(), img);
    } else {
        mw->showMessage("Editing gifs/video is unsupported.");
    }
}

void Core::crop(QRect rect) {
    if(!state.hasActiveImage)
        return;
    std::shared_ptr<Image> img = model->getItem(model->currentFileName());
    if(img && img->type() == STATIC) {
        auto imgStatic = dynamic_cast<ImageStatic *>(img.get());
        imgStatic->setEditedImage(std::unique_ptr<const QImage>(
                    ImageLib::cropped(imgStatic->getImage(), rect)));
        model->updateItem(model->currentFileName(), img);
    } else {
        mw->showMessage("Editing gifs/video is unsupported.");
    }
}

void Core::rotateByDegrees(int degrees) {
    if(!state.hasActiveImage)
        return;
    std::shared_ptr<Image> img = model->getItem(model->currentFileName());
    if(img && img->type() == STATIC) {
        auto imgStatic = dynamic_cast<ImageStatic *>(img.get());
        imgStatic->setEditedImage(std::unique_ptr<const QImage>(
                    ImageLib::rotated(imgStatic->getImage(), degrees)));
        model->updateItem(model->currentFileName(), img);
    } else {
        mw->showMessage("Editing gifs/video is unsupported.");
    }
}

void Core::discardEdits() {
    if(!state.hasActiveImage)
        return;
    std::shared_ptr<Image> img = model->getItem(model->currentFileName());
    if(img && img->type() == STATIC) {
        auto imgStatic = dynamic_cast<ImageStatic *>(img.get());
        imgStatic->discardEditedImage();
        model->updateItem(model->currentFileName(), img);
    }
    mw->hideSaveOverlay();
}

// move saving logic away from Image container itself
void Core::saveImageToDisk() {
    if(state.hasActiveImage)
        saveImageToDisk(model->currentFilePath());
}

void Core::saveImageToDisk(QString filePath) {
    if(!state.hasActiveImage)
        return;
    std::shared_ptr<Image> img = model->getItem(model->currentFileName());
    if(img->save(filePath))
        mw->showMessageSuccess("File saved.");
    else
        mw->showError("Could not save file.");
    mw->hideSaveOverlay();
}

void Core::sortByName() {
    auto mode = SortingMode::SORT_NAME;
    if(model->sortingMode() == mode)
        mode = SortingMode::SORT_NAME_DESC;
    model->setSortingMode(mode);
    mw->onSortingChanged(mode);
}

void Core::sortByTime() {
    auto mode = SortingMode::SORT_TIME;
    if(model->sortingMode() == mode)
        mode = SortingMode::SORT_TIME_DESC;
    model->setSortingMode(mode);
    mw->onSortingChanged(mode);
}

void Core::sortBySize() {
    auto mode = SortingMode::SORT_SIZE;
    if(model->sortingMode() == mode)
        mode = SortingMode::SORT_SIZE_DESC;
    model->setSortingMode(mode);
    mw->onSortingChanged(mode);
}

void Core::showRenameDialog() {
    if(!model->itemCount())
        return;
    mw->toggleRenameOverlay();
}

void Core::runScript(const QString &scriptName) {
    scriptManager->runScript(scriptName, model->cache.get(model->currentFileName()));
}

void Core::scalingRequest(QSize size, ScalingFilter filter) {
    if(state.hasActiveImage) {
        std::shared_ptr<Image> forScale = model->cache.get(model->currentFileName());
        if(forScale) {
            QString path = model->absolutePath() + "/" + model->currentFileName();
            model->scaler->requestScaled(ScalerRequest(forScale.get(), size, path, filter));
        }
    }
}

// TODO: don't use connect? otherwise there is no point using unique_ptr
void Core::onScalingFinished(QPixmap *scaled, ScalerRequest req) {
    if(state.hasActiveImage /* TODO: a better fix > */ && req.string == model->currentFilePath()) {
        mw->onScalingFinished(std::unique_ptr<QPixmap>(scaled));
    } else {
        delete scaled;
    }
}

void Core::trimCache() {
    QList<QString> list;
    list << model->prevOf(model->currentFileName());
    list << model->currentFileName();
    list << model->nextOf(model->currentFileName());
    model->cache.trimTo(list);
}

void Core::clearCache() {
    model->cache.clear();
}

// reset state; clear cache; etc
void Core::reset() {
    state.hasActiveImage = false;
    model->currentFileName() = "";
    //viewerWidget->unset();
    this->clearCache();
}

void Core::loadPath(QString path) {

    qDebug() << "loadPath(): " << path;

    if(path.startsWith("file://", Qt::CaseInsensitive))
        path.remove(0, 7);

    QFileInfo fileInfo(path);
    QString directoryPath;
    if(fileInfo.isDir()) {
        directoryPath = QDir(path).absolutePath();
    } else if(fileInfo.isFile()) {
        directoryPath = fileInfo.absolutePath();
    } else {
        mw->showError("Could not open path: " + path);
        qDebug() << "Could not open path: " << path;
        return;
    }
    qDebug() << "directoryPath: " << directoryPath;
    // set model dir if needed
    if(model->absolutePath() != directoryPath) {
        this->reset();
        settings->setLastDirectory(directoryPath);
        model->setDirectory(directoryPath);
        mw->setDirectoryPath(directoryPath);
    }
    // load file / folderview
    if(fileInfo.isFile()) {
        int index = model->indexOf(fileInfo.fileName());
        // DirectoryManager only checks file extensions via regex (performance reasons)
        // But in this case we force check mimetype
        if(index == -1) {
            QStringList types = settings->supportedMimeTypes();
            QMimeDatabase db;
            QMimeType type = db.mimeTypeForFile(fileInfo.filePath());
            if(types.contains(type.name())) {
                if(model->forceInsert(fileInfo.fileName())) {
                    index = model->indexOf(fileInfo.fileName());
                }
            }
        }
        model->setIndex(index);
    } else {
        //todo: set index without loading actual file?
        model->setIndex(0);
        mw->enableFolderView();
    }
}

void Core::nextImage() {
    if(model->isEmpty())
        return;
    if(settings->shuffleEnabled()) {
        model->setIndexAsync(randomizer.next());
        return;
    }

    int newIndex = model->indexOf(model->currentFileName()) + 1;
    if(newIndex >= model->itemCount()) {
        if(infiniteScrolling) {
            newIndex = 0;
        } else {
            if(!model->loaderBusy())
                mw->showMessageDirectoryEnd();
            return;
        }
    }
    model->setIndexAsync(newIndex);
}

void Core::prevImage() {
    if(model->isEmpty())
        return;
    if(settings->shuffleEnabled()) {
        model->setIndexAsync(randomizer.prev());
        return;
    }

    int newIndex = model->indexOf(model->currentFileName()) - 1;
    if(newIndex < 0) {
        if(infiniteScrolling) {
            newIndex = model->itemCount() - 1;
        } else {
            if(!model->loaderBusy())
                mw->showMessageDirectoryStart();
            return;
        }
    }
    model->setIndexAsync(newIndex);
}

void Core::jumpToFirst() {
    if(!model->isEmpty()) {
        model->setIndexAsync(0);
        mw->showMessageDirectoryStart();
    }
}

void Core::jumpToLast() {
    if(!model->isEmpty()) {
        model->setIndexAsync(model->itemCount() - 1);
        mw->showMessageDirectoryEnd();
    }
}

void Core::onLoadFailed(QString path) {
    Q_UNUSED(path)
    /*mw->showMessage("Load failed: " + path);
    QString currentPath = model->fullFilePath(model->currentFileName);
    if(path == currentPath)
        mw->closeImage();
        */
}

void Core::onModelItemReady(std::shared_ptr<Image> img) {
    displayImage(img);
    updateInfoString();
}

void Core::onModelItemUpdated(std::shared_ptr<Image> img) {
    onModelItemReady(img);
}

void Core::displayImage(std::shared_ptr<Image> img) {
    state.hasActiveImage = true;

    if(!img) {
        mw->showMessage("Error: could not load image.");
        return;
    }

    DocumentType type = img->type();
    if(type == STATIC) {
        mw->showImage(img->getPixmap());
    } else if(type == ANIMATED) {
        auto animated = dynamic_cast<ImageAnimated *>(img.get());
        mw->showAnimation(animated->getMovie());
    } else if(type == VIDEO) {
        auto video = dynamic_cast<Video *>(img.get());
        // workaround for mpv. If we play video while mainwindow is hidden we get black screen.
        // affects only initial startup (e.g. we open webm from file manager)
        showGui();
        mw->showVideo(video->getClip()->getPath());
    }
    img->isEdited() ? mw->showSaveOverlay() : mw->hideSaveOverlay();
    mw->setExifInfo(img->getExifTags());
}

void Core::updateInfoString() {
    QSize imageSize(0,0);
    int fileSize = 0;
    std::shared_ptr<Image> img = model->cache.get(model->currentFileName());
    if(img) {
        imageSize = img->size();
        fileSize  = img->fileSize();
    }
    mw->setCurrentInfo(model->indexOf(model->currentFileName()),
                       model->itemCount(),
                       model->currentFileName(),
                       imageSize,
                       fileSize);
}
