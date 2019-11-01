#include "directorymodel.h"

DirectoryModel::DirectoryModel(QObject *parent) : QObject(parent) {
    thumbnailer = new Thumbnailer(&dirManager); // remove pointers
    scaler = new Scaler(&cache);

    connect(&dirManager, &DirectoryManager::fileRemoved, this, &DirectoryModel::onFileRemoved);
    connect(&dirManager, &DirectoryManager::fileAdded, this, &DirectoryModel::fileAdded);
    connect(&dirManager, &DirectoryManager::fileModified,this, &DirectoryModel::fileModified);
    connect(&dirManager, &DirectoryManager::fileRenamed, this, &DirectoryModel::fileRenamed);
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

QString DirectoryModel::currentFileName() {
    return mCurrentFileName;
}

QString DirectoryModel::currentFilePath() {
    return dirManager.fullFilePath(mCurrentFileName);
}

int DirectoryModel::itemCount() const {
    return dirManager.fileCount();
}

int DirectoryModel::indexOf(QString fileName) {
    return dirManager.indexOf(fileName);
}

int DirectoryModel::currentIndex() {
    return dirManager.indexOf(mCurrentFileName);
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

QString DirectoryModel::fullPath(QString fileName) {
    return dirManager.fullFilePath(fileName);
}

bool DirectoryModel::contains(QString fileName) {
    return dirManager.contains(fileName);
}

void DirectoryModel::removeFile(QString fileName, bool trash, FileOpResult &result) {
    QFileInfo file(dirManager.directory() + "/" + fileName);
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

void DirectoryModel::copyTo(QString destDirectory, QString fileName, FileOpResult &result) {
    // error checks
    if(destDirectory == dirManager.directory()) {
        result = FileOpResult::COPY_TO_SAME_DIR;
        return;
    }
    if(!dirManager.contains(fileName)) { // if this happens we have a bug
        result = FileOpResult::SOURCE_DOES_NOT_EXIST;
        return;
    }
    QFileInfo location(destDirectory);
    if(!location.exists()) {
        result = FileOpResult::DESTINATION_DOES_NOT_EXIST;
        return;
    }
    if(!location.isWritable()) {
        result = FileOpResult::DESTINATION_NOT_WRITABLE;
        return;
    }
    location.setFile(destDirectory + "/" + fileName);
    if(location.exists()) {
        if(!location.isWritable())
            result = FileOpResult::DESTINATION_NOT_WRITABLE;
        result = FileOpResult::DESTINATION_FILE_EXISTS;
        return;
    }
    // copy
    if(dirManager.copyTo(destDirectory, fileName))
        result = FileOpResult::SUCCESS;
    else
        result = FileOpResult::OTHER_ERROR;
    return;
}

void DirectoryModel::moveTo(QString destDirectory, QString fileName, FileOpResult &result) {
    // error checks
    if(destDirectory == dirManager.directory()) {
        result = FileOpResult::COPY_TO_SAME_DIR;
        return;
    }
    if(!dirManager.contains(fileName)) { // if this happens we have a bug
        result = FileOpResult::SOURCE_DOES_NOT_EXIST;
        return;
    }
    QFileInfo location(destDirectory);
    if(!location.exists()) {
        result = FileOpResult::DESTINATION_DOES_NOT_EXIST;
        return;
    }
    if(!location.isWritable()) {
        result = FileOpResult::DESTINATION_NOT_WRITABLE;
        return;
    }
    location.setFile(destDirectory + "/" + fileName);
    if(location.exists()) {
        if(!location.isWritable())
            result = FileOpResult::DESTINATION_NOT_WRITABLE;
        result = FileOpResult::DESTINATION_FILE_EXISTS;
        return;
    }
    location.setFile(this->currentFilePath());
    if(!location.isWritable()) {
        result = FileOpResult::SOURCE_NOT_WRITABLE;
        return;
    }
    // move
    if(dirManager.copyTo(destDirectory, fileName)) {
        // remove original file
        if(dirManager.removeFile(fileName, false)) {
            // OK
            result = FileOpResult::SUCCESS;
            return;
        }
        // revert on failure
        result = FileOpResult::SOURCE_NOT_WRITABLE;
        if(QFile::remove(destDirectory + "/" + fileName))
            result = FileOpResult::OTHER_ERROR;
    } else {
        // could not COPY
        result = FileOpResult::OTHER_ERROR;
    }
    return;
}
// -----------------------------------------------------------------------------
void DirectoryModel::setDirectory(QString path) {
    dirManager.setDirectory(path);
}

QString DirectoryModel::directory() {
    return dirManager.directory();
}

bool DirectoryModel::setIndex(int index) {
    if(index >= 0 && index < itemCount()) {
        int oldIndex = currentIndex();
        QString newName = fileNameAt(index);
        if(mCurrentFileName != newName) {
            mCurrentFileName = fileNameAt(index);
            emit indexChanged(oldIndex, index);
            trimCache();
        }
        if(cache.contains(mCurrentFileName)) {
            if(settings->usePreloader()) {
                preload(dirManager.prevOf(mCurrentFileName));
                preload(dirManager.nextOf(mCurrentFileName));
            }
        } else {
            auto img = loader.load(fullPath(mCurrentFileName));
            cache.insert(img);
        }
        emit itemReady(cache.get(mCurrentFileName));
        return true;
    }
    return false;
}

bool DirectoryModel::setIndexAsync(int index) {
    if(index >= 0 && index < itemCount()) {
        QString newName = fileNameAt(index);
        int oldIndex = currentIndex();
        if(mCurrentFileName != newName) {
            mCurrentFileName = fileNameAt(index);
            emit indexChanged(oldIndex, index);
            trimCache();
        }
        if(cache.contains(mCurrentFileName)) {
            emit itemReady(cache.get(mCurrentFileName));
            if(settings->usePreloader()) {
                preload(dirManager.prevOf(mCurrentFileName));
                preload(dirManager.nextOf(mCurrentFileName));
            }
        } else {
            loader.loadAsyncPriority(fullPath(mCurrentFileName));
        }
        return true;
    }
    return false;
}

bool DirectoryModel::loaderBusy() {
    return loader.isBusy();
}

std::shared_ptr<Image> DirectoryModel::itemAt(int index) {
    return cache.get(dirManager.fileNameAt(index));
}

void DirectoryModel::trimCache() {
    QList<QString> list;
    list << prevOf(mCurrentFileName);
    list << mCurrentFileName;
    list << nextOf(mCurrentFileName);
    cache.trimTo(list);
}

void DirectoryModel::onItemReady(std::shared_ptr<Image> img) {
    if(!img)
        return;
    bool isRelevant = (img->name() == mCurrentFileName)
            || (img->name() == prevOf(mCurrentFileName))
            || (img->name() == nextOf(mCurrentFileName));
    if(isRelevant) {
        // maybe force insert into the cache? we already loaded it anyway
        if(!cache.insert(img)) {
            img = cache.get(img->name());
        }
    }
    if(img->name() == mCurrentFileName) {
        emit itemReady(img);
        if(settings->usePreloader()) {
            preload(dirManager.prevOf(mCurrentFileName));
            preload(dirManager.nextOf(mCurrentFileName));
        }
    }
}

void DirectoryModel::onSortingChanged() {
    trimCache();
    int index = dirManager.indexOf(mCurrentFileName);
    if(settings->usePreloader()) {
        preload(dirManager.prevOf(mCurrentFileName));
        preload(dirManager.nextOf(mCurrentFileName));
    }
    emit sortingChanged();
}

void DirectoryModel::onFileRemoved(QString fileName, int index) {
    if(!dirManager.fileCount())
        mCurrentFileName = "";
    emit fileRemoved(fileName, index);
}

std::shared_ptr<Image> DirectoryModel::getItemAt(int index) {
    return getItem(dirManager.fileNameAt(index));
}

// returns cached image
// if image is not cached, loads it in the main thread
// for async access use setIndex(int)
std::shared_ptr<Image> DirectoryModel::getItem(QString fileName) {
    std::shared_ptr<Image> img = cache.get(fileName);
    if(!img) {
        img = loader.load(fullPath(fileName));

    }
    return img;
}

void DirectoryModel::updateItem(QString fileName, std::shared_ptr<Image> img) {
    if(dirManager.contains(fileName)) {
        cache.insert(img);
        emit itemUpdated(img);
    }
}

void DirectoryModel::preload(QString fileName) {
    if(contains(fileName) && !cache.contains(fileName))
        loader.loadAsync(fullPath(fileName));
}
