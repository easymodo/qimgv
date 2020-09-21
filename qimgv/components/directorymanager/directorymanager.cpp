#include "directorymanager.h"

namespace fs = std::filesystem;

DirectoryManager::DirectoryManager() :
    watcher(nullptr),
    currentPath("")
{
    regex.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
    collator.setNumericMode(true);

    readSettings();
    connect(settings, &Settings::settingsChanged, this, &DirectoryManager::readSettings);
}

template< typename T, typename Pred >
typename std::vector<T>::iterator
insert_sorted(std::vector<T> & vec, T const& item, Pred pred) {
    return vec.insert(std::upper_bound(vec.begin(), vec.end(), item, pred), item);
}

bool DirectoryManager::path_entry_compare(const Entry &e1, const Entry &e2) const {
    return collator.compare(e1.path, e2.path) < 0;
};

bool DirectoryManager::path_entry_compare_reverse(const Entry &e1, const Entry &e2) const {
    return collator.compare(e1.path, e2.path) > 0;
};

bool DirectoryManager::name_entry_compare(const Entry &e1, const Entry &e2) const {
    return collator.compare(e1.name, e2.name) < 0;
};

bool DirectoryManager::name_entry_compare_reverse(const Entry &e1, const Entry &e2) const {
    return collator.compare(e1.name, e2.name) > 0;
};

bool DirectoryManager::date_entry_compare(const Entry& e1, const Entry& e2) const {
    return e1.modifyTime < e2.modifyTime;
}

bool DirectoryManager::date_entry_compare_reverse(const Entry& e1, const Entry& e2) const {
    return e1.modifyTime > e2.modifyTime;
}

bool DirectoryManager::size_entry_compare(const Entry& e1, const Entry& e2) const {
    return e1.size < e2.size;
}

bool DirectoryManager::size_entry_compare_reverse(const Entry& e1, const Entry& e2) const {
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

void DirectoryManager::startFileWatcher() {
    if(currentPath == "")
        return;
    if(!watcher)
        watcher = DirectoryWatcher::newInstance();

    connect(watcher, &DirectoryWatcher::fileCreated,  this, &DirectoryManager::onFileAddedExternal,    Qt::UniqueConnection);
    connect(watcher, &DirectoryWatcher::fileDeleted,  this, &DirectoryManager::onFileRemovedExternal,  Qt::UniqueConnection);
    connect(watcher, &DirectoryWatcher::fileModified, this, &DirectoryManager::onFileModifiedExternal, Qt::UniqueConnection);
    connect(watcher, &DirectoryWatcher::fileRenamed,  this, &DirectoryManager::onFileRenamedExternal,  Qt::UniqueConnection);

    watcher->setWatchPath(currentPath);
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
    filterRegex = settings->supportedFormatsRegex();
    regex.setPattern(filterRegex);
    setSortingMode(settings->sortingMode());
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
    currentPath = dirPath;
    generateFileList();
    sortFileList();
    emit loaded(dirPath);
    startFileWatcher();
    return true;
}

QString DirectoryManager::directoryPath() const {
    return currentPath;
}

int DirectoryManager::indexOf(QString filePath) const {
    auto item = find_if(entryVec.begin(), entryVec.end(), [filePath](const Entry& e) {
        return e.path == filePath;
    });

    if(item != entryVec.end()) {
        return distance(entryVec.begin(), item);
    }
    return -1;
}

// dumb. maybe better to store full paths in Entry right away
// !wont work with multiple directories. remove?
QString DirectoryManager::fullFilePath(QString fileName) const {
    return fileName.isEmpty() ? "" : currentPath + "/" + fileName;
}

QString DirectoryManager::filePathAt(int index) const {
    return checkRange(index) ? entryVec.at(index).path : "";
}

QString DirectoryManager::fileNameAt(int index) const {
    return checkRange(index) ? entryVec.at(index).name : "";
}

QString DirectoryManager::first() const {
    QString filePath = "";
    if(entryVec.size())
        filePath = entryVec.front().path;
    return filePath;
}

QString DirectoryManager::last() const {
    QString filePath = "";
    if(entryVec.size())
        filePath = entryVec.back().path;
    return filePath;
}

QString DirectoryManager::prevOf(QString filePath) const {
    QString prevFilePath = "";
    int currentIndex = indexOf(filePath);
    if(currentIndex > 0)
        prevFilePath = entryVec.at(currentIndex - 1).path;
    return prevFilePath;
}

QString DirectoryManager::nextOf(QString filePath) const {
    QString nextFilePath = "";
    int currentIndex = indexOf(filePath);
    if(currentIndex >= 0 && currentIndex < entryVec.size() - 1)
        nextFilePath = entryVec.at(currentIndex + 1).path;
    return nextFilePath;
}

bool DirectoryManager::removeFile(QString filePath, bool trash) {
    if(!contains(filePath))
        return false;
    QFile file(filePath);
    int index = indexOf(filePath);
    if(trash) {
        if(moveToTrash(filePath)) {
            entryVec.erase(entryVec.begin() + index);
            emit fileRemoved(filePath, index);
            return true;
        }
    } else if(file.remove()) {
        entryVec.erase(entryVec.begin() + index);
        emit fileRemoved(filePath, index);
        return true;
    }
    return false;
}

#ifdef Q_OS_WIN32
bool DirectoryManager::moveToTrash(QString file) {
    QFileInfo fileinfo( file );
    if( !fileinfo.exists() )
        return false;
    WCHAR from[ MAX_PATH ];
    memset( from, 0, sizeof( from ));
    int l = fileinfo.absoluteFilePath().toWCharArray( from );
    Q_ASSERT( 0 <= l && l < MAX_PATH );
    from[ l ] = '\0';
    SHFILEOPSTRUCTW fileop;
    memset( &fileop, 0, sizeof( fileop ) );
    fileop.wFunc = FO_DELETE;
    fileop.pFrom = from;
    fileop.fFlags = FOF_ALLOWUNDO | FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT;
    int rv = SHFileOperationW( &fileop );
    if( 0 != rv ){
        qDebug() << rv << QString::number( rv ).toInt( nullptr, 8 );
        qDebug() << "move to trash failed";
        return false;
    }
    return true;
}
#endif

#ifdef Q_OS_LINUX
bool DirectoryManager::moveToTrash(QString filePath) {
    #ifdef QT_GUI_LIB
    bool TrashInitialized = false;
    QString TrashPath;
    QString TrashPathInfo;
    QString TrashPathFiles;
    if(!TrashInitialized) {
        QStringList paths;
        const char* xdg_data_home = getenv( "XDG_DATA_HOME" );
        if(xdg_data_home) {
            qDebug() << "XDG_DATA_HOME not yet tested";
            QString xdgTrash( xdg_data_home );
            paths.append(xdgTrash + "/Trash");
        }
        QString home = QStandardPaths::writableLocation( QStandardPaths::HomeLocation );
        paths.append( home + "/.local/share/Trash" );
        paths.append( home + "/.trash" );
        foreach( QString path, paths ){
            if( TrashPath.isEmpty() ){
                QDir dir( path );
                if( dir.exists() ){
                    TrashPath = path;
                }
            }
        }
        if( TrashPath.isEmpty() )
            qDebug() << "Can`t detect trash folder";
        TrashPathInfo = TrashPath + "/info";
        TrashPathFiles = TrashPath + "/files";
        if( !QDir( TrashPathInfo ).exists() || !QDir( TrashPathFiles ).exists() )
            qDebug() << "Trash doesn`t look like FreeDesktop.org Trash specification";
        TrashInitialized = true;
    }
    QFileInfo original( filePath );
    if( !original.exists() )
        qDebug() << "File doesn`t exist, cant move to trash";
    QString info;
    info += "[Trash Info]\nPath=";
    info += original.absoluteFilePath();
    info += "\nDeletionDate=";
    info += QDateTime::currentDateTime().toString("yyyy-MM-ddThh:mm:ss");
    info += "\n";
    QString trashname = original.fileName();
    QString infopath = TrashPathInfo + "/" + trashname + ".trashinfo";
    QString filepath = TrashPathFiles + "/" + trashname;
    int nr = 1;
    while( QFileInfo( infopath ).exists() || QFileInfo( filepath ).exists() ){
        nr++;
        trashname = original.baseName() + "." + QString::number( nr );
        if( !original.completeSuffix().isEmpty() ){
            trashname += QString( "." ) + original.completeSuffix();
        }
        infopath = TrashPathInfo + "/" + trashname + ".trashinfo";
        filepath = TrashPathFiles + "/" + trashname;
    }
    QDir dir;
    if( !dir.rename( original.absoluteFilePath(), filepath ) ){
        qDebug() << "move to trash failed";
    }
    QFile infoFile(infopath);
    infoFile.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&infoFile);
    out.setCodec("UTF-8");
    out.setGenerateByteOrderMark(false);
    out << info;
    infoFile.close();
    #else
    Q_UNUSED( file );
    qDebug() << "Trash in server-mode not supported";
    #endif
    return true;
}
#endif

#ifdef Q_OS_MAC
bool DirectoryManager::moveToTrash(QString file) { // todo
    return false;
}
#endif

bool DirectoryManager::checkRange(int index) const {
    return index >= 0 && index < (int)entryVec.size();
}

unsigned long DirectoryManager::fileCount() const {
    return entryVec.size();
}

bool DirectoryManager::isDirectory(QString path) const {
    QFileInfo fileInfo(path);
    return (fileInfo.isDir() && fileInfo.isReadable());
}

QDateTime DirectoryManager::lastModified(QString filePath) const {
    QFileInfo info;
    if(contains(filePath))
        info.setFile(filePath);
    return info.lastModified();
}

// TODO: what about symlinks?
inline
bool DirectoryManager::isSupportedFile(QString path) const {
    return ( isFile(path) && regex.match(path).hasMatch() );
}

inline
bool DirectoryManager::isFile(QString path) const {
    if(!std::filesystem::exists(toStdString(path)))
        return false;
    if(!std::filesystem::is_regular_file(toStdString(path)))
        return false;
    return true;
}

bool DirectoryManager::isEmpty() const {
    return entryVec.empty();
}

bool DirectoryManager::contains(QString filePath) const {
    return (std::find(entryVec.begin(), entryVec.end(), filePath) != entryVec.end());
}

// ##############################################################
// ###################### PRIVATE METHODS #######################
// ##############################################################
void DirectoryManager::generateFileList() {
    entryVec.clear();
    QRegularExpressionMatch match;
    for(const auto & entry : fs::directory_iterator(toStdString(currentPath))) {
        QString name = QString::fromStdString(entry.path().filename().string());
        QString path = QString::fromStdString(entry.path().string());
        match = regex.match(name);
        if(match.hasMatch()) {
            Entry newEntry;
            try {
                newEntry.name = name;
                newEntry.path = path;
                newEntry.size = entry.file_size();
                newEntry.modifyTime = entry.last_write_time();
                newEntry.isDirectory = entry.is_directory();
            } catch (const std::filesystem::filesystem_error &err) {
                qDebug() << "[DirectoryManager]" << err.what();
                continue;
            }
            entryVec.emplace_back(newEntry);
            /* It is probably worth implementing lazy loading in future.
             * Read names only, and other stuff ondemand.
             * This would give ~2x load speedup when we are just sorting by filename.
             */
            //entryVec.emplace_back(Entry(name, entry.is_directory()));
        }
    }
}

void DirectoryManager::sortFileList() {
    sort(entryVec.begin(), entryVec.end(), std::bind(compareFunction(), this, std::placeholders::_1, std::placeholders::_2));
}

void DirectoryManager::setSortingMode(SortingMode mode) {
    if(mode != mSortingMode) {
        mSortingMode = mode;
        if(entryVec.size() > 1) {
            sortFileList();
            emit sortingChanged();
        }
    }
}

SortingMode DirectoryManager::sortingMode() const {
    return mSortingMode;
}

// ? fixme
bool DirectoryManager::forceInsert(QString filePath) {
    if(!this->isFile(filePath) || contains(filePath))
        return false;
    std::filesystem::directory_entry stdEntry(toStdString(filePath));
    QString fileName = QString::fromStdString(stdEntry.path().filename().string()); // isn't it beautiful
    Entry entry(filePath, fileName, stdEntry.file_size(), stdEntry.last_write_time(), stdEntry.is_directory());
    insert_sorted(entryVec, entry, std::bind(compareFunction(), this, std::placeholders::_1, std::placeholders::_2));
    emit fileAdded(filePath);
    return true;
}

//----------------------------------------------------------------------------
// fs watcher events  ( onFile___External() )
// these take file NAMES, not paths
void DirectoryManager::onFileRemovedExternal(QString fileName) {
    QString filePath = watcher->watchPath() + "/" + fileName;
    if(!contains(filePath))
        return;
    QFile file(filePath);
    if(file.exists())
        return;
    int index = indexOf(filePath);
    entryVec.erase(entryVec.begin() + index);
    emit fileRemoved(filePath, index);
}

void DirectoryManager::onFileAddedExternal(QString fileName) {
    QString filePath = watcher->watchPath() + "/" + fileName;
    if(!isSupportedFile(filePath))
        return;
    if(contains(filePath))
        onFileModifiedExternal(fileName);
    std::filesystem::directory_entry stdEntry(toStdString(filePath));
    Entry entry(filePath, fileName, stdEntry.file_size(), stdEntry.last_write_time(), stdEntry.is_directory());
    insert_sorted(entryVec, entry, std::bind(compareFunction(), this, std::placeholders::_1, std::placeholders::_2));
    emit fileAdded(filePath);
    return;
}

void DirectoryManager::onFileRenamedExternal(QString oldFileName, QString newFileName) {
    QString oldFilePath = watcher->watchPath() + "/" + oldFileName;
    QString newFilePath = watcher->watchPath() + "/" + newFileName;
    if(!contains(oldFilePath)) {
        if(contains(newFilePath))
            onFileModifiedExternal(newFileName);
        else
            onFileAddedExternal(newFileName);
        return;
    }
    if(!this->isSupportedFile(newFilePath)) {
        onFileRemovedExternal(oldFileName);
        return;
    }
    if(contains(newFilePath)) {
        int replaceIndex = indexOf(newFilePath);
        entryVec.erase(entryVec.begin() + replaceIndex);
        emit fileRemoved(newFilePath, replaceIndex);
    }
    // remove the old one
    int oldIndex = indexOf(oldFilePath);
    entryVec.erase(entryVec.begin() + oldIndex);
    // insert
    std::filesystem::directory_entry stdEntry(toStdString(newFilePath));
    Entry entry(newFilePath, newFileName, stdEntry.file_size(), stdEntry.last_write_time(), stdEntry.is_directory());
    insert_sorted(entryVec, entry, std::bind(compareFunction(), this, std::placeholders::_1, std::placeholders::_2));
    emit fileRenamed(oldFilePath, oldIndex, newFilePath, indexOf(newFilePath));
}

void DirectoryManager::onFileModifiedExternal(QString fileName) {
    QString filePath = watcher->watchPath() + "/" + fileName;
    if(!contains(filePath))
        return;
    std::filesystem::directory_entry stdEntry(toStdString(filePath));
    int index = indexOf(filePath);
    if(entryVec.at(index).modifyTime != stdEntry.last_write_time())
        entryVec.at(index).modifyTime = stdEntry.last_write_time();
    emit fileModified(filePath);
}
