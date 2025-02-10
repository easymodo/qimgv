#include "directorymanager.h"

namespace fs = std::filesystem;

DirectoryManager::DirectoryManager() :
    watcher(nullptr),
    mSortingMode(SORT_NAME)
{
    regex.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
    collator.setNumericMode(true);

    readSettings();
    setSortingMode(settings->sortingMode());
    connect(settings, &Settings::settingsChanged, this, &DirectoryManager::readSettings);
}

template< typename T, typename Pred >
typename std::vector<T>::iterator
insert_sorted(std::vector<T> & vec, T const& item, Pred pred) {
    return vec.insert(std::upper_bound(vec.begin(), vec.end(), item, pred), item);
}

bool DirectoryManager::path_entry_compare(const FSEntry &e1, const FSEntry &e2) const {
    return collator.compare(e1.path, e2.path) < 0;
};

bool DirectoryManager::path_entry_compare_reverse(const FSEntry &e1, const FSEntry &e2) const {
    return collator.compare(e1.path, e2.path) > 0;
};

bool DirectoryManager::name_entry_compare(const FSEntry &e1, const FSEntry &e2) const {
    return collator.compare(e1.name, e2.name) < 0;
};

bool DirectoryManager::name_entry_compare_reverse(const FSEntry &e1, const FSEntry &e2) const {
    return collator.compare(e1.name, e2.name) > 0;
};

bool DirectoryManager::date_entry_compare(const FSEntry& e1, const FSEntry& e2) const {
    return e1.modifyTime < e2.modifyTime;
}

bool DirectoryManager::date_entry_compare_reverse(const FSEntry& e1, const FSEntry& e2) const {
    return e1.modifyTime > e2.modifyTime;
}

bool DirectoryManager::size_entry_compare(const FSEntry& e1, const FSEntry& e2) const {
    return e1.size < e2.size;
}

bool DirectoryManager::size_entry_compare_reverse(const FSEntry& e1, const FSEntry& e2) const {
    return e1.size > e2.size;
}

CompareFunction DirectoryManager::compareFunction() {
    CompareFunction cmpFn = &DirectoryManager::path_entry_compare;
    if(mSortingMode == SortingMode::SORT_NAME_DESC)
        cmpFn = &DirectoryManager::path_entry_compare_reverse;
    if(mSortingMode == SortingMode::SORT_TIME)
        cmpFn = &DirectoryManager::date_entry_compare;
    if(mSortingMode == SortingMode::SORT_TIME_DESC)
        cmpFn = &DirectoryManager::date_entry_compare_reverse;
    if(mSortingMode == SortingMode::SORT_SIZE)
        cmpFn = &DirectoryManager::size_entry_compare;
    if(mSortingMode == SortingMode::SORT_SIZE_DESC)
        cmpFn = &DirectoryManager::size_entry_compare_reverse;
    return cmpFn;
}

void DirectoryManager::startFileWatcher(QString directoryPath) {
    if(directoryPath == "")
        return;
    if(!watcher)
        watcher = DirectoryWatcher::newInstance();

    connect(watcher, &DirectoryWatcher::fileCreated,  this, &DirectoryManager::onFileAddedExternal,    Qt::UniqueConnection);
    connect(watcher, &DirectoryWatcher::fileDeleted,  this, &DirectoryManager::onFileRemovedExternal,  Qt::UniqueConnection);
    connect(watcher, &DirectoryWatcher::fileModified, this, &DirectoryManager::onFileModifiedExternal, Qt::UniqueConnection);
    connect(watcher, &DirectoryWatcher::fileRenamed,  this, &DirectoryManager::onFileRenamedExternal,  Qt::UniqueConnection);

    watcher->setWatchPath(directoryPath);
    watcher->observe();
}

void DirectoryManager::stopFileWatcher() {
    if(!watcher)
        return;

    watcher->stopObserving();

    disconnect(watcher, &DirectoryWatcher::fileCreated,  this, &DirectoryManager::onFileAddedExternal);
    disconnect(watcher, &DirectoryWatcher::fileDeleted,  this, &DirectoryManager::onFileRemovedExternal);
    disconnect(watcher, &DirectoryWatcher::fileModified, this, &DirectoryManager::onFileModifiedExternal);
    disconnect(watcher, &DirectoryWatcher::fileRenamed,  this, &DirectoryManager::onFileRenamedExternal);
}

// ##############################################################
// ####################### PUBLIC METHODS #######################
// ##############################################################

void DirectoryManager::readSettings() {
    regex.setPattern(settings->supportedFormatsRegex());
}

bool DirectoryManager::setDirectory(QString dirPath) {
    if(dirPath.isEmpty()) {
        return false;
    }
    if(!std::filesystem::exists(toStdString(dirPath))) {
        qDebug() << "[DirectoryManager] Error - path does not exist.";
        return false;
    }
    if(!std::filesystem::is_directory(toStdString(dirPath))) {
        qDebug() << "[DirectoryManager] Error - path is not a directory.";
        return false;
    }
    QDir dir(dirPath);
    if(!dir.isReadable()) {
        qDebug() << "[DirectoryManager] Error - cannot read directory.";
        return false;
    }
    mListSource = SOURCE_DIRECTORY;
    mDirectoryPath = dirPath;

    loadEntryList(dirPath, false);
    sortEntryLists();
    emit loaded(dirPath);
    startFileWatcher(dirPath);
    return true;
}

bool DirectoryManager::setDirectoryRecursive(QString dirPath) {
    if(dirPath.isEmpty()) {
        return false;
    }
    if(!std::filesystem::exists(toStdString(dirPath))) {
        qDebug() << "[DirectoryManager] Error - path does not exist.";
        return false;
    }
    if(!std::filesystem::is_directory(toStdString(dirPath))) {
        qDebug() << "[DirectoryManager] Error - path is not a directory.";
        return false;
    }
    stopFileWatcher();
    mListSource = SOURCE_DIRECTORY_RECURSIVE;
    mDirectoryPath = dirPath;
    loadEntryList(dirPath, true);
    sortEntryLists();
    emit loaded(dirPath);
    return true;
}

QString DirectoryManager::directoryPath() const {
    if(mListSource == SOURCE_DIRECTORY || mListSource == SOURCE_DIRECTORY_RECURSIVE)
        return mDirectoryPath;
    else
        return "";
}

int DirectoryManager::indexOfFile(QString filePath) const {
    auto item = find_if(fileEntryVec.begin(), fileEntryVec.end(), [filePath](const FSEntry& e) {
        return e.path == filePath;
    });
    if(item != fileEntryVec.end())
        return distance(fileEntryVec.begin(), item);
    return -1;
}

int DirectoryManager::indexOfDir(QString dirPath) const {
    auto item = find_if(dirEntryVec.begin(), dirEntryVec.end(), [dirPath](const FSEntry& e) {
        return e.path == dirPath;
    });
    if(item != dirEntryVec.end())
        return distance(dirEntryVec.begin(), item);
    return -1;
}

QString DirectoryManager::filePathAt(int index) const {
    return checkFileRange(index) ? fileEntryVec.at(index).path : "";
}

QString DirectoryManager::fileNameAt(int index) const {
    return checkFileRange(index) ? fileEntryVec.at(index).name : "";
}

QString DirectoryManager::dirPathAt(int index) const {
    return checkDirRange(index) ? dirEntryVec.at(index).path : "";
}

QString DirectoryManager::dirNameAt(int index) const {
    return checkDirRange(index) ? dirEntryVec.at(index).name : "";
}

QString DirectoryManager::firstFile() const {
    QString filePath = "";
    if(fileEntryVec.size())
        filePath = fileEntryVec.front().path;
    return filePath;
}

QString DirectoryManager::lastFile() const {
    QString filePath = "";
    if(fileEntryVec.size())
        filePath = fileEntryVec.back().path;
    return filePath;
}

QString DirectoryManager::prevOfFile(QString filePath) const {
    QString prevFilePath = "";
    int currentIndex = indexOfFile(filePath);
    if(currentIndex > 0)
        prevFilePath = fileEntryVec.at(currentIndex - 1).path;
    return prevFilePath;
}

QString DirectoryManager::nextOfFile(QString filePath) const {
    QString nextFilePath = "";
    int currentIndex = indexOfFile(filePath);
    if(currentIndex >= 0 && currentIndex < fileEntryVec.size() - 1)
        nextFilePath = fileEntryVec.at(currentIndex + 1).path;
    return nextFilePath;
}

QString DirectoryManager::prevOfDir(QString dirPath) const {
    QString prevDirectoryPath = "";
    int currentIndex = indexOfDir(dirPath);
    if(currentIndex > 0)
        prevDirectoryPath = dirEntryVec.at(currentIndex - 1).path;
    return prevDirectoryPath;
}

QString DirectoryManager::nextOfDir(QString dirPath) const {
    QString nextDirectoryPath = "";
    int currentIndex = indexOfDir(dirPath);
    if(currentIndex >= 0 && currentIndex < dirEntryVec.size() - 1)
        nextDirectoryPath = dirEntryVec.at(currentIndex + 1).path;
    return nextDirectoryPath;
}

bool DirectoryManager::checkFileRange(int index) const {
    return index >= 0 && index < (int)fileEntryVec.size();
}

bool DirectoryManager::checkDirRange(int index) const {
    return index >= 0 && index < (int)dirEntryVec.size();
}

unsigned long DirectoryManager::totalCount() const {
    return fileCount() + dirCount();
}

unsigned long DirectoryManager::fileCount() const {
    return fileEntryVec.size();
}

unsigned long DirectoryManager::dirCount() const {
    return dirEntryVec.size();
}

const FSEntry &DirectoryManager::fileEntryAt(int index) const {
    if(checkFileRange(index))
        return fileEntryVec.at(index);
    else
        return defaultEntry;
}

QDateTime DirectoryManager::lastModified(QString filePath) const {
    QFileInfo info;
    if(containsFile(filePath))
        info.setFile(filePath);
    return info.lastModified();
}

// TODO: what about symlinks?
inline
bool DirectoryManager::isSupportedFile(QString path) const {
    return ( isFile(path) && regex.match(path).hasMatch() );
}

bool DirectoryManager::isFile(QString path) const {
    if(!std::filesystem::exists(toStdString(path)))
        return false;
    if(!std::filesystem::is_regular_file(toStdString(path)))
        return false;
    return true;
}

bool DirectoryManager::isDir(QString path) const {
    if(!std::filesystem::exists(toStdString(path)))
        return false;
    if(!std::filesystem::is_directory(toStdString(path)))
        return false;
    return true;
}

bool DirectoryManager::isEmpty() const {
    return fileEntryVec.empty();
}

bool DirectoryManager::containsFile(QString filePath) const {
    return (std::find(fileEntryVec.begin(), fileEntryVec.end(), filePath) != fileEntryVec.end());
}

bool DirectoryManager::containsDir(QString dirPath) const {
    return (std::find(dirEntryVec.begin(), dirEntryVec.end(), dirPath) != dirEntryVec.end());
}

// ##############################################################
// ###################### PRIVATE METHODS #######################
// ##############################################################
void DirectoryManager::loadEntryList(QString directoryPath, bool recursive) {
    dirEntryVec.clear();
    fileEntryVec.clear();
    if(recursive) { // load files only
        addEntriesFromDirectoryRecursive(fileEntryVec, directoryPath);
    } else { // load dirs & files
        addEntriesFromDirectory(fileEntryVec, directoryPath);
    }
}

// both directories & files
void DirectoryManager::addEntriesFromDirectory(std::vector<FSEntry> &entryVec, QString directoryPath) {
    QRegularExpressionMatch match;
    for(const auto & entry : fs::directory_iterator(toStdString(directoryPath))) {
        QString name = QString::fromStdString(entry.path().filename().generic_string());
#ifndef Q_OS_WIN32
        // ignore hidden files
        if(!settings->showHiddenFiles() && name.startsWith("."))
            continue;
#else
        DWORD attributes = GetFileAttributes(entry.path().generic_string().c_str());
        if(!settings->showHiddenFiles() && attributes & FILE_ATTRIBUTE_HIDDEN)
            continue;
#endif
        QString path = QString::fromStdString(entry.path().generic_string());
        match = regex.match(name);
        if(entry.is_directory()) { // this can still throw std::bad_alloc ..
            FSEntry newEntry;
            try {
                newEntry.name = name;
                newEntry.path = path;
                newEntry.isDirectory = true;
                //newEntry.size = entry.file_size();
                //newEntry.modifyTime = entry.last_write_time();
            } catch (const std::filesystem::filesystem_error &err) {
                qDebug() << "[DirectoryManager]" << err.what();
                continue;
            }
            dirEntryVec.emplace_back(newEntry);
        } else if (match.hasMatch()) {
            FSEntry newEntry;
            try {
                newEntry.name = name;
                newEntry.path = path;
                newEntry.isDirectory = false;
                newEntry.size = entry.file_size();
                newEntry.modifyTime = entry.last_write_time();
            } catch (const std::filesystem::filesystem_error &err) {
                qDebug() << "[DirectoryManager]" << err.what();
                continue;
            }
            entryVec.emplace_back(newEntry);
        }
    }
}

void DirectoryManager::addEntriesFromDirectoryRecursive(std::vector<FSEntry> &entryVec, QString directoryPath) {
    QRegularExpressionMatch match;
    for(const auto & entry : fs::recursive_directory_iterator(toStdString(directoryPath))) {
        QString name = QString::fromStdString(entry.path().filename().generic_string());
        QString path = QString::fromStdString(entry.path().generic_string());
        match = regex.match(name);
        if(!entry.is_directory() && match.hasMatch()) {
            FSEntry newEntry;
            try {
                newEntry.name = name;
                newEntry.path = path;
                newEntry.isDirectory = false;
                newEntry.size = entry.file_size();
                newEntry.modifyTime = entry.last_write_time();
            } catch (const std::filesystem::filesystem_error &err) {
                qDebug() << "[DirectoryManager]" << err.what();
                continue;
            }
            entryVec.emplace_back(newEntry);
        }
    }
}

void DirectoryManager::sortEntryLists() {
    if(settings->sortFolders())
        std::sort(dirEntryVec.begin(), dirEntryVec.end(), std::bind(compareFunction(), this, std::placeholders::_1, std::placeholders::_2));
    else
        std::sort(dirEntryVec.begin(), dirEntryVec.end(), std::bind(&DirectoryManager::path_entry_compare, this, std::placeholders::_1, std::placeholders::_2));
    std::sort(fileEntryVec.begin(), fileEntryVec.end(), std::bind(compareFunction(), this, std::placeholders::_1, std::placeholders::_2));
}

void DirectoryManager::setSortingMode(SortingMode mode) {
    if(mode != mSortingMode) {
        mSortingMode = mode;
        if(fileEntryVec.size() > 1 || dirEntryVec.size() > 1) {
            sortEntryLists();
            emit sortingChanged();
        }
    }
}

SortingMode DirectoryManager::sortingMode() const {
    return mSortingMode;
}

// Entry management

bool DirectoryManager::insertFileEntry(const QString &filePath) {
    if(!isSupportedFile(filePath))
        return false;
    return forceInsertFileEntry(filePath);
}

// skips filename regex check
bool DirectoryManager::forceInsertFileEntry(const QString &filePath) {
    if(!this->isFile(filePath) || containsFile(filePath))
        return false;
    std::filesystem::directory_entry stdEntry(toStdString(filePath));
    QString fileName = QString::fromStdString(stdEntry.path().filename().generic_string()); // isn't it beautiful
    FSEntry FSEntry(filePath, fileName, stdEntry.file_size(), stdEntry.last_write_time(), stdEntry.is_directory());
    insert_sorted(fileEntryVec, FSEntry, std::bind(compareFunction(), this, std::placeholders::_1, std::placeholders::_2));
    if(!directoryPath().isEmpty()) {
        qDebug() << "fileIns" << filePath << directoryPath();
        emit fileAdded(filePath);
    }
    return true;
}

void DirectoryManager::removeFileEntry(const QString &filePath) {
    if(!containsFile(filePath))
        return;
    int index = indexOfFile(filePath);
    fileEntryVec.erase(fileEntryVec.begin() + index);
    qDebug() << "fileRem" << filePath;
    emit fileRemoved(filePath, index);
}

void DirectoryManager::updateFileEntry(const QString &filePath) {
    if(!containsFile(filePath))
        return;
    FSEntry newEntry(filePath);
    int index = indexOfFile(filePath);
    if(fileEntryVec.at(index).modifyTime != newEntry.modifyTime)
        fileEntryVec.at(index) = newEntry;
    qDebug() << "fileMod" << filePath;
    emit fileModified(filePath);
}

void DirectoryManager::renameFileEntry(const QString &oldFilePath, const QString &newFileName) {
    QFileInfo fi(oldFilePath);
    QString newFilePath = fi.absolutePath() + "/" + newFileName;
    if(!containsFile(oldFilePath)) {
        if(containsFile(newFilePath))
            updateFileEntry(newFilePath);
        else
            insertFileEntry(newFilePath);
        return;
    }
    if(!isSupportedFile(newFilePath)) {
        removeFileEntry(oldFilePath);
        return;
    }
    if(containsFile(newFilePath)) {
        int replaceIndex = indexOfFile(newFilePath);
        fileEntryVec.erase(fileEntryVec.begin() + replaceIndex);
        emit fileRemoved(newFilePath, replaceIndex);
    }
    // remove the old one
    int oldIndex = indexOfFile(oldFilePath);
    fileEntryVec.erase(fileEntryVec.begin() + oldIndex);
    // insert
    std::filesystem::directory_entry stdEntry(toStdString(newFilePath));
    FSEntry FSEntry(newFilePath, newFileName, stdEntry.file_size(), stdEntry.last_write_time(), stdEntry.is_directory());
    insert_sorted(fileEntryVec, FSEntry, std::bind(compareFunction(), this, std::placeholders::_1, std::placeholders::_2));
    qDebug() << "fileRen" << oldFilePath << newFilePath;
    emit fileRenamed(oldFilePath, oldIndex, newFilePath, indexOfFile(newFilePath));
}

// ---- dir entries

bool DirectoryManager::insertDirEntry(const QString &dirPath) {
    if(containsDir(dirPath))
        return false;
    std::filesystem::directory_entry stdEntry(toStdString(dirPath));
    QString dirName = QString::fromStdString(stdEntry.path().filename().generic_string()); // isn't it beautiful
    FSEntry FSEntry;
    FSEntry.name = dirName;
    FSEntry.path = dirPath;
    FSEntry.isDirectory = true;
    insert_sorted(dirEntryVec, FSEntry, std::bind(compareFunction(), this, std::placeholders::_1, std::placeholders::_2));
    qDebug() << "dirIns" << dirPath;
    emit dirAdded(dirPath);
    return true;
}

void DirectoryManager::removeDirEntry(const QString &dirPath) {
    if(!containsDir(dirPath))
        return;
    int index = indexOfDir(dirPath);
    dirEntryVec.erase(dirEntryVec.begin() + index);
    qDebug() << "dirRem" << dirPath;
    emit dirRemoved(dirPath, index);
}

void DirectoryManager::renameDirEntry(const QString &oldDirPath, const QString &newDirName) {
    if(!containsDir(oldDirPath))
        return;
    QFileInfo fi(oldDirPath);
    QString newDirPath = fi.absolutePath() + "/" + newDirName;
    // remove the old one
    int oldIndex = indexOfDir(oldDirPath);
    dirEntryVec.erase(dirEntryVec.begin() + oldIndex);
    // insert
    std::filesystem::directory_entry stdEntry(toStdString(newDirPath));
    FSEntry FSEntry;
    FSEntry.name = newDirName;
    FSEntry.path = newDirPath;
    FSEntry.isDirectory = true;
    insert_sorted(dirEntryVec, FSEntry, std::bind(compareFunction(), this, std::placeholders::_1, std::placeholders::_2));
    qDebug() << "dirRen" << oldDirPath << newDirPath;
    emit dirRenamed(oldDirPath, oldIndex, newDirPath, indexOfDir(newDirPath));
}


FileListSource DirectoryManager::source() const {
    return mListSource;
}

QStringList DirectoryManager::fileList() const {
    QStringList list;
    for(auto const& value : fileEntryVec)
        list << value.path;
    return list;
}

bool DirectoryManager::fileWatcherActive() {
    if(!watcher)
        return false;
    return watcher->isObserving();
}

//----------------------------------------------------------------------------
// fs watcher events  ( onFile___External() )
// these take file NAMES, not paths
void DirectoryManager::onFileRemovedExternal(QString fileName) {
    QString fullPath = watcher->watchPath() + "/" + fileName;
    removeDirEntry(fullPath);
    removeFileEntry(fullPath);
}

void DirectoryManager::onFileAddedExternal(QString fileName) {
    QString fullPath = watcher->watchPath() + "/" + fileName;
    if(isDir(fullPath))
        insertDirEntry(fullPath);
    else
        insertFileEntry(fullPath);
}

void DirectoryManager::onFileRenamedExternal(QString oldName, QString newName) {
    QString oldPath = watcher->watchPath() + "/" + oldName;
    QString newPath = watcher->watchPath() + "/" + newName;
    if(isDir(newPath))
        renameDirEntry(oldPath, newName);
    else
        renameFileEntry(oldPath, newName);
}

void DirectoryManager::onFileModifiedExternal(QString fileName) {
    updateFileEntry(watcher->watchPath() + "/" + fileName);
}
