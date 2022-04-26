#include "directorymodel.h"

DirectoryModel::DirectoryModel(QObject *parent) :
    QObject(parent),
    fileListSource(SOURCE_DIRECTORY)
{
    scaler = new Scaler(&cache);

    connect(&dirManager, &DirectoryManager::fileRemoved,  this, &DirectoryModel::onFileRemoved);
    connect(&dirManager, &DirectoryManager::fileAdded,    this, &DirectoryModel::onFileAdded);
    connect(&dirManager, &DirectoryManager::fileRenamed,  this, &DirectoryModel::onFileRenamed);
    connect(&dirManager, &DirectoryManager::fileModified, this, &DirectoryModel::onFileModified);
    connect(&dirManager, &DirectoryManager::dirRemoved,  this, &DirectoryModel::dirRemoved);
    connect(&dirManager, &DirectoryManager::dirAdded,    this, &DirectoryModel::dirAdded);
    connect(&dirManager, &DirectoryManager::dirRenamed,  this, &DirectoryModel::dirRenamed);

    connect(&dirManager, &DirectoryManager::loaded, this, &DirectoryModel::loaded);
    connect(&dirManager, &DirectoryManager::sortingChanged, this, &DirectoryModel::onSortingChanged);
    connect(&loader, &Loader::loadFinished, this, &DirectoryModel::onImageReady);
    connect(&loader, &Loader::loadFailed, this, &DirectoryModel::loadFailed);
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

const FSEntry &DirectoryModel::fileEntryAt(int index) const {
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

bool DirectoryModel::autoRefresh() {
    return dirManager.fileWatcherActive();
}

FileListSource DirectoryModel::source() {
    return dirManager.source();
}

QString DirectoryModel::directoryPath() const {
    return dirManager.directoryPath();
}

bool DirectoryModel::containsFile(QString filePath) const {
    return dirManager.containsFile(filePath);
}

bool DirectoryModel::containsDir(QString dirPath) const {
    return dirManager.containsDir(dirPath);
}

bool DirectoryModel::isEmpty() const {
    return dirManager.isEmpty();
}

QString DirectoryModel::firstFile() const {
    return dirManager.firstFile();
}

QString DirectoryModel::lastFile() const {
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
    return dirManager.forceInsertFileEntry(filePath);
}

void DirectoryModel::setSortingMode(SortingMode mode) {
    dirManager.setSortingMode(mode);
}

void DirectoryModel::removeFile(const QString &filePath, bool trash, FileOpResult &result) {
    if(trash)
        FileOperations::moveToTrash(filePath, result);
    else
        FileOperations::removeFile(filePath, result);
    if(result != FileOpResult::SUCCESS)
        return;
    dirManager.removeFileEntry(filePath);
    return;
}

void DirectoryModel::renameEntry(const QString &oldPath, const QString &newName, bool force, FileOpResult &result) {
    bool isDir = dirManager.isDir(oldPath);
    FileOperations::rename(oldPath, newName, force, result);
    // chew through watcher events so they wont be processed out of order
    qApp->processEvents();
    if(result != FileOpResult::SUCCESS)
        return;
    if(isDir)
        dirManager.renameDirEntry(oldPath, newName);
    else
        dirManager.renameFileEntry(oldPath, newName);
}

void DirectoryModel::removeDir(const QString &dirPath, bool trash, bool recursive, FileOpResult &result) {
    if(trash) {
        FileOperations::moveToTrash(dirPath, result);
    } else {
        FileOperations::removeDir(dirPath, recursive, result);
    }
    if(result != FileOpResult::SUCCESS)
        return;
    dirManager.removeDirEntry(dirPath);
    return;
}

void DirectoryModel::copyFileTo(const QString &srcFile, const QString &destDirPath, bool force, FileOpResult &result) {
    FileOperations::copyFileTo(srcFile, destDirPath, force, result);
}

void DirectoryModel::moveFileTo(const QString &srcFile, const QString &destDirPath, bool force, FileOpResult &result) {
    FileOperations::moveFileTo(srcFile, destDirPath, force, result);
    // chew through watcher events so they wont be processed out of order
    qApp->processEvents();
    if(result == FileOpResult::SUCCESS) {
        if(destDirPath != this->directoryPath())
            dirManager.removeFileEntry(srcFile);
    }
}
// -----------------------------------------------------------------------------
bool DirectoryModel::setDirectory(QString path) {
    cache.clear();
    return dirManager.setDirectory(path);
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

void DirectoryModel::onImageReady(std::shared_ptr<Image> img, const QString &path) {
    if(!img) {
        emit loadFailed(path);
        return;
    }
    cache.remove(path);
    cache.insert(img);
    emit imageReady(img, path);
}

bool DirectoryModel::saveFile(const QString &filePath) {
    return saveFile(filePath, filePath);
}

bool DirectoryModel::saveFile(const QString &filePath, const QString &destPath) {
    if(!containsFile(filePath) || !cache.contains(filePath))
        return false;
    auto img = cache.get(filePath);
    if(img->save(destPath)) {
        if(filePath == destPath) { // replace
            dirManager.updateFileEntry(destPath);
            emit fileModified(destPath);
        } else { // manually add if we are saving to the same dir
            QFileInfo fiSrc(filePath);
            QFileInfo fiDest(destPath);
            // handle same dir
            if(fiSrc.absolutePath() == fiDest.absolutePath()) {
                // overwrite
                if(!dirManager.containsFile(destPath) && dirManager.insertFileEntry(destPath))
                    emit fileModified(destPath);
            }
        }
        return true;
    }
    return false;
}

// dirManager events

void DirectoryModel::onSortingChanged() {
    emit sortingChanged(sortingMode());
}

void DirectoryModel::onFileAdded(QString filePath) {
    emit fileAdded(filePath);
}

void DirectoryModel::onFileModified(QString filePath) {
    QDateTime modTime = lastModified(filePath);
    if(modTime.isValid()) {
        auto img = cache.get(filePath);
        if(img) {
            // check if file on disk is different
            if(modTime != img->lastModified())
                reload(filePath);
        }
        emit fileModified(filePath);
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
// for async access use loadAsync(), then catch onImageReady()
std::shared_ptr<Image> DirectoryModel::getImage(QString filePath) {
    std::shared_ptr<Image> img = cache.get(filePath);
    if(!img)
        img = loader.load(filePath);
    return img;
}

void DirectoryModel::updateImage(QString filePath, std::shared_ptr<Image> img) {
    if(containsFile(filePath) /*& cache.contains(filePath)*/) {
        if(!cache.contains(filePath)) {
            cache.insert(img);
        } else {
            cache.insert(img);
            emit imageUpdated(filePath);
        }
    }
}

void DirectoryModel::load(QString filePath, bool asyncHint) {
    if(!containsFile(filePath) || loader.isLoading(filePath))
        return;
    if(!cache.contains(filePath)) {
        if(asyncHint) {
            loader.loadAsyncPriority(filePath);
        } else {
            auto img = loader.load(filePath);
            if(img) {
                cache.insert(img);
                emit imageReady(img, filePath);
            } else {
                emit loadFailed(filePath);
            }
        }
    } else {
        emit imageReady(cache.get(filePath), filePath);
    }
}

void DirectoryModel::reload(QString filePath) {
    if(cache.contains(filePath)) {
        cache.remove(filePath);
        dirManager.updateFileEntry(filePath);
        load(filePath, false);
    }
}

void DirectoryModel::preload(QString filePath) {
    if(containsFile(filePath) && !cache.contains(filePath))
        loader.loadAsync(filePath);
}
