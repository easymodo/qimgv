#include "directorymodel.h"

DirectoryModel::DirectoryModel(QObject *parent) :
    QObject(parent),
    fileListSource(SOURCE_DIRECTORY)
{
    scaler = new Scaler(&cache);

    connect(&dirManager, &DirectoryManager::fileRemoved, this, &DirectoryModel::onFileRemoved);
    connect(&dirManager, &DirectoryManager::fileAdded, this, &DirectoryModel::onFileAdded);
    connect(&dirManager, &DirectoryManager::fileModified,this, &DirectoryModel::onFileModified);
    connect(&dirManager, &DirectoryManager::fileRenamed, this, &DirectoryModel::onFileRenamed);
    connect(&dirManager, &DirectoryManager::loaded, this, &DirectoryModel::loaded);
    connect(&dirManager, &DirectoryManager::sortingChanged, this, &DirectoryModel::onSortingChanged);
    connect(&loader, &Loader::loadFinished, this, &DirectoryModel::onImageReady);
}

DirectoryModel::~DirectoryModel() {
    loader.clearTasks();
    delete scaler;
}

int DirectoryModel::totalCount() const {
    return dirManager.totalCount();
}

int DirectoryModel::fileCount() const {
    return dirManager.fileCount();
}

int DirectoryModel::dirCount() const {
    return dirManager.dirCount();
}

int DirectoryModel::indexOfFile(QString filePath) const {
    return dirManager.indexOfFile(filePath);
}

int DirectoryModel::indexOfDir(QString filePath) const {
    return dirManager.indexOfDir(filePath);
}

SortingMode DirectoryModel::sortingMode() const {
    return dirManager.sortingMode();
}

const FSEntry &DirectoryModel::entryAt(int index) const {
    return dirManager.fileEntryAt(index);
}

QString DirectoryModel::fileNameAt(int index) const {
    return dirManager.fileNameAt(index);
}

QString DirectoryModel::filePathAt(int index) const {
    return dirManager.filePathAt(index);
}

QString DirectoryModel::dirNameAt(int index) const {
    return dirManager.dirNameAt(index);
}

QString DirectoryModel::dirPathAt(int index) const {
    return dirManager.dirPathAt(index);
}

QString DirectoryModel::directoryPath() const {
    return dirManager.directoryPath();
}

bool DirectoryModel::contains(QString filePath) const {
    return dirManager.containsFile(filePath);
}

bool DirectoryModel::isEmpty() const {
    return dirManager.isEmpty();
}

QString DirectoryModel::first() const {
    return dirManager.firstFile();
}

QString DirectoryModel::last() const {
    return dirManager.lastFile();
}

QString DirectoryModel::nextOf(QString filePath) const {
    return dirManager.nextOfFile(filePath);
}

QString DirectoryModel::prevOf(QString filePath) const {
    return dirManager.prevOfFile(filePath);
}

QDateTime DirectoryModel::lastModified(QString filePath) const {
    return dirManager.lastModified(filePath);
}

// -----------------------------------------------------------------------------

bool DirectoryModel::forceInsert(QString filePath) {
    return dirManager.forceInsertFile(filePath);
}

void DirectoryModel::setSortingMode(SortingMode mode) {
    dirManager.setSortingMode(mode);
}

void DirectoryModel::removeFile(QString filePath, bool trash, FileOpResult &result) {
    QFileInfo file(filePath);
    if(!file.exists()) {
        result = FileOpResult::SOURCE_DOES_NOT_EXIST;
    } else if(!file.isWritable()) {
        result = FileOpResult::SOURCE_NOT_WRITABLE;
    } else {
        if(dirManager.removeFile(filePath, trash))
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
    if(!contains(srcFile.absoluteFilePath())) { // if this happens we have a bug
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
    if(!contains(srcFile.absoluteFilePath())) { // if this happens we have a bug
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
        removeFile(srcFile.absoluteFilePath(), false, removeResult);
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
    QString filePath = this->filePathAt(index);
    cache.remove(filePath);
}

void DirectoryModel::unload(QString filePath) {
    cache.remove(filePath);
}

void DirectoryModel::unloadExcept(QString filePath, bool keepNearby) {
    QList<QString> list;
    list << filePath;
    if(keepNearby)  {
        list << prevOf(filePath);
        list << nextOf(filePath);
    }
    cache.trimTo(list);
}

bool DirectoryModel::loaderBusy() const {
    return loader.isBusy();
}

void DirectoryModel::onImageReady(std::shared_ptr<Image> img) {
    if(!img)
        return;
    cache.remove(img->path());
    cache.insert(img);
    emit imageReady(img);
}

void DirectoryModel::onSortingChanged() {
    emit sortingChanged(sortingMode());
}

void DirectoryModel::onFileAdded(QString filePath) {
    emit fileAdded(filePath);
}

void DirectoryModel::onFileModified(QString filePath) {
    QDateTime modTime = lastModified(filePath);
    if(modTime.isValid()) {
        // modTime will mismatch if it was modified from outside
        auto img = cache.get(filePath);
        if(!img) {
            emit fileModified(filePath);
        } else if(modTime > img->lastModified()) {
            reload(filePath);
            emit fileModified(filePath);
        } else {
            emit fileModifiedInternal(filePath);
        }
    }
}

void DirectoryModel::onFileRemoved(QString filePath, int index) {
    unload(filePath);
    emit fileRemoved(filePath, index);
}

void DirectoryModel::onFileRenamed(QString fromPath, int indexFrom, QString toPath, int indexTo) {
    unload(fromPath);
    emit fileRenamed(fromPath, indexFrom, toPath, indexTo);
}

bool DirectoryModel::isLoaded(int index) const {
    return cache.contains(filePathAt(index));
}

bool DirectoryModel::isLoaded(QString filePath) const {
    return cache.contains(filePath);
}

std::shared_ptr<Image> DirectoryModel::getImageAt(int index) {
    return getImage(filePathAt(index));
}

// returns cached image
// if image is not cached, loads it in the main thread
// for async access use setIndexAsync(int)
std::shared_ptr<Image> DirectoryModel::getImage(QString filePath) {
    std::shared_ptr<Image> img = cache.get(filePath);
    if(!img)
        img = loader.load(filePath);
    return img;
}

void DirectoryModel::updateImage(QString filePath, std::shared_ptr<Image> img) {
    if(contains(filePath)) {
        cache.insert(img);
        emit imageUpdated(filePath);
    }
}

void DirectoryModel::load(QString filePath, bool asyncHint) {
    if(!contains(filePath) || loader.isLoading(filePath))
        return;
    if(!cache.contains(filePath)) {
        if(asyncHint) {
            loader.loadAsyncPriority(filePath);
        } else {
            auto img = loader.load(filePath);
            cache.insert(img);
            emit imageReady(img);
        }
    } else {
        emit imageReady(cache.get(filePath));
    }
}

void DirectoryModel::reload(QString filePath) {
    if(cache.contains(filePath)) {
        cache.remove(filePath);
        load(filePath, false);
    }
}

void DirectoryModel::preload(QString filePath) {
    if(contains(filePath) && !cache.contains(filePath))
        loader.loadAsync(filePath);
}
