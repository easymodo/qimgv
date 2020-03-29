#include "directorymodel.h"

DirectoryModel::DirectoryModel(QObject *parent) : QObject(parent) {
    thumbnailer = new Thumbnailer(&dirManager);
    scaler = new Scaler(&cache);

    connect(&dirManager, &DirectoryManager::fileRemoved, this, &DirectoryModel::onFileRemoved);
    connect(&dirManager, &DirectoryManager::fileAdded, this, &DirectoryModel::onFileAdded);
    connect(&dirManager, &DirectoryManager::fileModified,this, &DirectoryModel::onFileModified);
    connect(&dirManager, &DirectoryManager::fileRenamed, this, &DirectoryModel::onFileRenamed);
    connect(&dirManager, &DirectoryManager::loaded, this, &DirectoryModel::loaded);
    connect(&dirManager, &DirectoryManager::sortingChanged, this, &DirectoryModel::onSortingChanged);
    connect(&loader, &Loader::loadFinished, this, &DirectoryModel::onItemReady);
    connect(thumbnailer, &Thumbnailer::thumbnailReady, this, &DirectoryModel::thumbnailReady);
    connect(this, &DirectoryModel::generateThumbnails, thumbnailer, &Thumbnailer::generateThumbnails);
}

DirectoryModel::~DirectoryModel() {
    thumbnailer->clearTasks();
    loader.clearTasks();
    delete scaler;
    delete thumbnailer;
}

int DirectoryModel::itemCount() const {
    return dirManager.fileCount();
}

int DirectoryModel::indexOf(QString fileName) {
    return dirManager.indexOf(fileName);
}

SortingMode DirectoryModel::sortingMode() {
    return dirManager.sortingMode();
}

bool DirectoryModel::forceInsert(QString fileName) {
    return dirManager.forceInsert(fileName);
}

void DirectoryModel::setSortingMode(SortingMode mode) {
    dirManager.setSortingMode(mode);
}

QString DirectoryModel::fileNameAt(int index) {
    return dirManager.fileNameAt(index);
}

QString DirectoryModel::filePathAt(int index) {
    return dirManager.filePathAt(index);
}

QString DirectoryModel::fullPath(QString fileName) {
    return dirManager.fullFilePath(fileName);
}

bool DirectoryModel::contains(QString fileName) {
    return dirManager.contains(fileName);
}

void DirectoryModel::removeFile(QString fileName, bool trash, FileOpResult &result) {
    QFileInfo file(directoryPath() + "/" + fileName);
    if(!file.exists()) {
        result = FileOpResult::SOURCE_DOES_NOT_EXIST;
    } else if(!file.isWritable()) {
        result = FileOpResult::SOURCE_NOT_WRITABLE;
    } else {
        if(dirManager.removeFile(fileName, trash))
            result = FileOpResult::SUCCESS;
        else
            result = FileOpResult::OTHER_ERROR;
    }
    return;
}

bool DirectoryModel::isEmpty() {
    return dirManager.isEmpty();
}

QString DirectoryModel::nextOf(QString fileName) {
    return dirManager.nextOf(fileName);
}

QString DirectoryModel::prevOf(QString fileName) {
    return dirManager.prevOf(fileName);
}

QString DirectoryModel::first() {
    return dirManager.first();
}

QString DirectoryModel::last() {
    return dirManager.last();
}

QString DirectoryModel::absolutePath() {
    return dirManager.absolutePath();
}

QDateTime DirectoryModel::lastModified(QString fileName) {
    return dirManager.lastModified(fileName);
}

void DirectoryModel::copyTo(QString destDir, QUrl srcUrl, FileOpResult &result) {
    QString srcName = srcUrl.fileName();
    // error checks
    if(destDir == dirManager.directoryPath()) {
        result = FileOpResult::COPY_TO_SAME_DIR;
        return;
    }
    if(!dirManager.contains(srcName)) { // if this happens we have a bug
        result = FileOpResult::SOURCE_DOES_NOT_EXIST;
        return;
    }
    QFileInfo location(destDir);
    if(!location.exists()) {
        result = FileOpResult::DESTINATION_DOES_NOT_EXIST;
        return;
    }
    if(!location.isWritable()) {
        result = FileOpResult::DESTINATION_NOT_WRITABLE;
        return;
    }
    location.setFile(destDir + "/" + srcName);
    if(location.exists()) {
        if(!location.isWritable())
            result = FileOpResult::DESTINATION_NOT_WRITABLE;
        result = FileOpResult::DESTINATION_FILE_EXISTS;
        return;
    }
    // copy
    if(dirManager.copyTo(destDir, srcName))
        result = FileOpResult::SUCCESS;
    else
        result = FileOpResult::OTHER_ERROR;
    return;
}

void DirectoryModel::moveTo(QString destDir, QUrl srcUrl, FileOpResult &result) {
    QString srcName = srcUrl.fileName();
    // error checks
    if(destDir == dirManager.directoryPath()) {
        result = FileOpResult::COPY_TO_SAME_DIR;
        return;
    }
    if(!dirManager.contains(srcName)) { // if this happens we have a bug
        result = FileOpResult::SOURCE_DOES_NOT_EXIST;
        return;
    }
    QFileInfo location(destDir);
    if(!location.exists()) {
        result = FileOpResult::DESTINATION_DOES_NOT_EXIST;
        return;
    }
    if(!location.isWritable()) {
        result = FileOpResult::DESTINATION_NOT_WRITABLE;
        return;
    }
    location.setFile(destDir + "/" + srcName);
    if(location.exists()) {
        if(!location.isWritable())
            result = FileOpResult::DESTINATION_NOT_WRITABLE;
        result = FileOpResult::DESTINATION_FILE_EXISTS;
        return;
    }
    location.setFile(srcUrl.toString());
    if(!location.isWritable()) {
        result = FileOpResult::SOURCE_NOT_WRITABLE;
        return;
    }
    // move
    if(dirManager.copyTo(destDir, srcName)) {
        // remove original file
        if(dirManager.removeFile(srcName, false)) {
            // OK
            result = FileOpResult::SUCCESS;
            return;
        }
        // revert on failure
        result = FileOpResult::SOURCE_NOT_WRITABLE;
        if(QFile::remove(destDir + "/" + srcName))
            result = FileOpResult::OTHER_ERROR;
    } else {
        // could not COPY
        result = FileOpResult::OTHER_ERROR;
    }
    return;
}
// -----------------------------------------------------------------------------
void DirectoryModel::setDirectory(QString path) {
    cache.clear();
    dirManager.setDirectory(path);
}

QString DirectoryModel::directoryPath() {
    return dirManager.directoryPath();
}

void DirectoryModel::unload(int index) {
    QString fileName = this->fileNameAt(index);
    cache.remove(fileName);
}

void DirectoryModel::unload(QString fileName) {
    cache.remove(fileName);
}

void DirectoryModel::unloadExcept(QString fileName, bool keepNearby) {
    QList<QString> list;
    list << fileName;
    if(keepNearby)  {
        list << prevOf(fileName);
        list << nextOf(fileName);
    }
    cache.trimTo(list);
}

bool DirectoryModel::loaderBusy() {
    return loader.isBusy();
}

std::shared_ptr<Image> DirectoryModel::itemAt(int index) {
    return cache.get(fileNameAt(index));
}

void DirectoryModel::onItemReady(std::shared_ptr<Image> img) {
    if(!img)
        return;
    cache.remove(img->name());
    cache.insert(img);
    emit itemReady(img);
}

void DirectoryModel::onSortingChanged() {
    //trimCache();
    if(settings->usePreloader()) {
     //   preload(dirManager.prevOf(mCurrentFileName));
     //   preload(dirManager.nextOf(mCurrentFileName));
    }
    emit sortingChanged();
}

void DirectoryModel::onFileAdded(QString fileName) {
    emit fileAdded(fileName);
}

void DirectoryModel::onFileModified(QString fileName) {
    QDateTime modTime = lastModified(fileName);
    if(modTime.isValid()) {
        // modTime will mismatch if it was modified from outside
        auto img = cache.get(fileName);
        if(!img) {
            emit fileModified(fileName);
        } else if(modTime > img->lastModified()) {
            reload(fileName);
            emit fileModified(fileName);
        } else {
            emit fileModifiedInternal(fileName);
        }
    }
}

void DirectoryModel::onFileRemoved(QString fileName, int index) {
    unload(fileName);
    emit fileRemoved(fileName, index);
}

void DirectoryModel::onFileRenamed(QString from, int indexFrom, QString to, int indexTo) {
    unload(from);
    emit fileRenamed(from, indexFrom, to, indexTo);
}

bool DirectoryModel::isLoaded(int index) {
    return cache.contains(fileNameAt(index));
}

bool DirectoryModel::isLoaded(QString fileName) {
    return cache.contains(fileName);
}

std::shared_ptr<Image> DirectoryModel::getItemAt(int index) {
    return getItem(fileNameAt(index));
}

// returns cached image
// if image is not cached, loads it in the main thread
// for async access use setIndexAsync(int)
std::shared_ptr<Image> DirectoryModel::getItem(QString fileName) {
    std::shared_ptr<Image> img = cache.get(fileName);
    if(!img)
        img = loader.load(fullPath(fileName));
    return img;
}

void DirectoryModel::updateItem(QString fileName, std::shared_ptr<Image> img) {
    if(contains(fileName)) {
        cache.insert(img);
        emit itemUpdated(fileName);
    }
}

void DirectoryModel::load(QString fileName, bool asyncHint) {
    if(!contains(fileName) || loader.isLoading(fullPath(fileName)))
        return;
    if(!cache.contains(fileName)) {
        if(asyncHint) {
            loader.loadAsyncPriority(fullPath(fileName));
        } else {
            auto img = loader.load(fullPath(fileName));
            cache.insert(img);
            emit itemReady(img);
        }
    } else {
        emit itemReady(cache.get(fileName));
    }
}

void DirectoryModel::reload(QString fileName) {
    if(cache.contains(fileName)) {
        cache.remove(fileName);
        load(fileName, false);
    }
}

void DirectoryModel::preload(QString fileName) {
    if(contains(fileName) && !cache.contains(fileName))
        loader.loadAsync(fullPath(fileName));
}
