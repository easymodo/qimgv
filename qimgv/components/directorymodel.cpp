#include "directorymodel.h"

DirectoryModel::DirectoryModel(QObject *parent) :
    QObject(parent),
    fileListSource(SOURCE_DIRECTORY)
{
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

int DirectoryModel::indexOf(QString fileName) const {
    return dirManager.indexOf(fileName);
}

SortingMode DirectoryModel::sortingMode() const {
    return dirManager.sortingMode();
}

QString DirectoryModel::fileNameAt(int index) const {
    return dirManager.fileNameAt(index);
}

QString DirectoryModel::filePathAt(int index) const {
    return dirManager.filePathAt(index);
}

QString DirectoryModel::fullPath(QString fileName) const {
    return dirManager.fullFilePath(fileName);
}

QString DirectoryModel::directoryPath() const {
    return dirManager.directoryPath();
}

bool DirectoryModel::contains(QString fileName) const {
    return dirManager.contains(fileName);
}

bool DirectoryModel::isEmpty() const {
    return dirManager.isEmpty();
}

QString DirectoryModel::first() const {
    return dirManager.first();
}

QString DirectoryModel::last() const {
    return dirManager.last();
}

QString DirectoryModel::nextOf(QString fileName) const {
    return dirManager.nextOf(fileName);
}

QString DirectoryModel::prevOf(QString fileName) const {
    return dirManager.prevOf(fileName);
}

QDateTime DirectoryModel::lastModified(QString fileName) const {
    return dirManager.lastModified(fileName);
}

// -----------------------------------------------------------------------------

bool DirectoryModel::forceInsert(QString fileName) {
    return dirManager.forceInsert(fileName);
}

void DirectoryModel::setSortingMode(SortingMode mode) {
    dirManager.setSortingMode(mode);
}

void DirectoryModel::removeFile(QString fileName, bool trash, FileOpResult &result) {
    QFileInfo file(fullPath(fileName));
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

void DirectoryModel::copyTo(const QString destDirPath, const QFileInfo srcFile, FileOpResult &result) {
    // error checks
    if(destDirPath == directoryPath()) {
        result = FileOpResult::COPY_TO_SAME_DIR;
        return;
    }
    if(!contains(srcFile.fileName())) { // if this happens we have a bug
        result = FileOpResult::SOURCE_DOES_NOT_EXIST;
        return;
    }
    QFileInfo destDir(destDirPath);
    if(!destDir.exists()) {
        result = FileOpResult::DESTINATION_DOES_NOT_EXIST;
        return;
    }
    if(!destDir.isWritable()) {
        result = FileOpResult::DESTINATION_NOT_WRITABLE;
        return;
    }
    QFileInfo destFile(destDirPath + "/" + srcFile.fileName());
    if(destFile.exists()) {
        if(!destFile.isWritable())
            result = FileOpResult::DESTINATION_NOT_WRITABLE;
        result = FileOpResult::DESTINATION_FILE_EXISTS;
        return;
    }
    // copy
    if(QFile::copy(srcFile.absoluteFilePath(), destFile.absoluteFilePath()))
        result = FileOpResult::SUCCESS;
    else
        result = FileOpResult::OTHER_ERROR;
    return;
}

void DirectoryModel::moveTo(const QString destDirPath, const QFileInfo srcFile, FileOpResult &result) {
    // error checks
    if(destDirPath == directoryPath()) {
        result = FileOpResult::COPY_TO_SAME_DIR;
        return;
    }
    if(!contains(srcFile.fileName())) { // if this happens we have a bug
        result = FileOpResult::SOURCE_DOES_NOT_EXIST;
        return;
    }
    QFileInfo destDir(destDirPath);
    if(!destDir.exists()) {
        result = FileOpResult::DESTINATION_DOES_NOT_EXIST;
        return;
    }
    if(!destDir.isWritable()) {
        result = FileOpResult::DESTINATION_NOT_WRITABLE;
        return;
    }
    QFileInfo destFile(destDirPath + "/" + srcFile.fileName());
    if(destFile.exists()) {
        if(!destFile.isWritable())
            result = FileOpResult::DESTINATION_NOT_WRITABLE;
        result = FileOpResult::DESTINATION_FILE_EXISTS;
        return;
    }
    if(!srcFile.isWritable()) {
        result = FileOpResult::SOURCE_NOT_WRITABLE;
        return;
    }
    // move
    if(QFile::copy(srcFile.absoluteFilePath(), destFile.absoluteFilePath())) {
        // remove original file
        FileOpResult removeResult;
        removeFile(srcFile.fileName(), false, removeResult);
        if(removeResult == FileOpResult::SUCCESS) {
            // OK
            result = FileOpResult::SUCCESS;
            return;
        }
        // revert on failure
        result = FileOpResult::SOURCE_NOT_WRITABLE;
        if(QFile::remove(destFile.absoluteFilePath()))
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

bool DirectoryModel::loaderBusy() const {
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
    emit sortingChanged(sortingMode());
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

bool DirectoryModel::isLoaded(int index) const {
    return cache.contains(fileNameAt(index));
}

bool DirectoryModel::isLoaded(QString fileName) const {
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
