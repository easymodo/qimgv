#include "directorymanager.h"

namespace fs = std::filesystem;

DirectoryManager::DirectoryManager() {
    currentPath = "";

    regex.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
    collator.setNumericMode(true);

    watcher = DirectoryWatcher::newInstance();

    connect(watcher, &DirectoryWatcher::observingStarted, this, [] () {
    //    qDebug() << "observing started";
    });
    connect(watcher, &DirectoryWatcher::observingStopped, this, [] () {
    //    qDebug() << "observing stopped";
    });
    connect(watcher, &DirectoryWatcher::fileCreated, this, [this] (const QString& filename) {
    //    qDebug() << "[w] file created" << filename;
        onFileAddedExternal(filename);
    });
    connect(watcher, &DirectoryWatcher::fileDeleted, this, [this] (const QString& filename) {
    //    qDebug() << "[w] file deleted" << filename;
        onFileRemovedExternal(filename);
    });
    connect(watcher, &DirectoryWatcher::fileModified, this, [this] (const QString& filename) {
    //    qDebug() << "[w] file modified" << filename;
        onFileModifiedExternal(filename);
    });
    connect(watcher, &DirectoryWatcher::fileRenamed, this, [this] (const QString& file1, const QString& file2) {
    //    qDebug() << "[w] file renamed from" << file1 << "to" << file2;
        onFileRenamedExternal(file1, file2);
    });

    readSettings();
    connect(settings, &Settings::settingsChanged, this, &DirectoryManager::readSettings);
}

template< typename T, typename Pred >
typename std::vector<T>::iterator
insert_sorted(std::vector<T> & vec, T const& item, Pred pred) {
    return vec.insert(std::upper_bound(vec.begin(), vec.end(), item, pred), item);
}

bool DirectoryManager::name_entry_compare(const Entry &e1, const Entry &e2) const {
    return collator.compare(e1.path, e2.path) < 0;
};

bool DirectoryManager::name_entry_compare_reverse(const Entry &e1, const Entry &e2) const {
    return collator.compare(e1.path, e2.path) > 0;
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

bool DirectoryManager::entryCompareString(Entry &e, QString path) {
    return (e.path == path);
}

CompareFunction DirectoryManager::compareFunction() {
    CompareFunction cmpFn = &DirectoryManager::name_entry_compare;
    if(mSortingMode == SortingMode::SORT_NAME_DESC)
        cmpFn = &DirectoryManager::name_entry_compare_reverse;
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

// ##############################################################
// ####################### PUBLIC METHODS #######################
// ##############################################################

void DirectoryManager::readSettings() {
    filterRegex = settings->supportedFormatsRegex();
    regex.setPattern(filterRegex);
    setSortingMode(settings->sortingMode());
}

bool DirectoryManager::setDirectory(QString path) {
    if(path.isEmpty()) {
        qDebug() << "[DirectoryManager] Error - path is empty.";
        return false;
    }
    if(!std::filesystem::exists(toStdString(path))) {
        qDebug() << "[DirectoryManager] Error - path does not exist.";
        return false;
    }
    if(!std::filesystem::is_directory(toStdString(path))) {
        qDebug() << "[DirectoryManager] Error - path is not a directory.";
        return false;
    }
    currentPath = path;
    generateFileList();
    sortFileList();

    emit loaded(path);

    watcher->setWatchPath(path);
    watcher->observe();

    return true;
}

int DirectoryManager::indexOf(QString fileName) const {
    auto item = find_if(entryVec.begin(), entryVec.end(), [fileName](const Entry& e) {
        return e.path == fileName;
    });

    if(item != entryVec.end()) {
        return distance(entryVec.begin(), item);
    }
    return -1;
}

QString DirectoryManager::absolutePath() const {
    return currentPath;
}

QString DirectoryManager::filePathAt(int index) const {
    return checkRange(index) ? currentPath + "/" + entryVec.at(index).path : "";
}

// dumb. maybe better to store full paths in Entry right away
QString DirectoryManager::fullFilePath(QString fileName) const {
    return fileName.isEmpty() ? "" : currentPath + "/" + fileName;
}

QString DirectoryManager::fileNameAt(int index) const {
    return checkRange(index) ? entryVec.at(index).path : "";
}

QString DirectoryManager::first() {
    QString fileName = "";
    if(entryVec.size())
        fileName = entryVec.front().path;
    return fileName;
}

QString DirectoryManager::last() {
    QString fileName = "";
    if(entryVec.size())
        fileName = entryVec.back().path;
    return fileName;
}

QString DirectoryManager::prevOf(QString fileName) const {
    QString prevFileName = "";
    int currentIndex = indexOf(fileName);
    if(currentIndex > 0)
        prevFileName = entryVec.at(currentIndex - 1).path;
    return prevFileName;
}

QString DirectoryManager::nextOf(QString fileName) const {
    QString nextFileName = "";
    int currentIndex = indexOf(fileName);
    if(currentIndex >= 0 && currentIndex < entryVec.size() - 1)
        nextFileName = entryVec.at(currentIndex + 1).path;
    return nextFileName;
}

bool DirectoryManager::removeFile(QString fileName, bool trash) {
    if(!contains(fileName))
        return false;
    QString path = fullFilePath(fileName);
    QFile file(path);
    int index = indexOf(fileName);
    if(trash) {
        entryVec.erase(entryVec.begin() + index);
        moveToTrash(path);
        emit fileRemoved(fileName, index);
        return true;
    } else if(file.remove()) {
        entryVec.erase(entryVec.begin() + index);
        emit fileRemoved(fileName, index);
        return true;
    }
    return false;
}

#ifdef Q_OS_WIN32
void DirectoryManager::moveToTrash(QString file) {
    QFileInfo fileinfo( file );
    if( !fileinfo.exists() )
        qDebug() << "File doesnt exists, cant move to trash";
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
    }
}
#endif

#ifdef Q_OS_LINUX
void DirectoryManager::moveToTrash(QString file) {
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
    QFileInfo original( file );
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
}
#endif

bool DirectoryManager::checkRange(int index) const {
    return index >= 0 && index < (int)entryVec.size();
}

bool DirectoryManager::copyTo(QString destDirectory, QString fileName) {
    if(!contains(fileName))
        return false;
    return QFile::copy(fullFilePath(fileName), destDirectory + "/" + fileName);
}

unsigned long DirectoryManager::fileCount() const {
    return entryVec.size();
}

bool DirectoryManager::isDirectory(QString path) const {
    QFileInfo fileInfo(path);
    return (fileInfo.isDir() && fileInfo.isReadable());
}

QDateTime DirectoryManager::lastModified(QString fileName) const {
    QFileInfo info;
    if(contains(fileName))
        info.setFile(fullFilePath(fileName));
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

bool DirectoryManager::contains(QString fileName) const {
    return (std::find(entryVec.begin(), entryVec.end(), fileName) != entryVec.end());
}

// ##############################################################
// ###################### PRIVATE METHODS #######################
// ##############################################################
void DirectoryManager::generateFileList() {
    entryVec.clear();
    QElapsedTimer t;
    t.start();
    QRegularExpressionMatch match;
    for(const auto & entry : fs::directory_iterator(toStdString(currentPath))) {
        QString name = QString::fromStdString(entry.path().filename().string());
        match = regex.match(name);
        if(match.hasMatch()) {
            Entry newEntry;
            try {
                newEntry.path = name;
                newEntry.size = entry.file_size();
                newEntry.modifyTime = entry.last_write_time();
                newEntry.isDirectory = entry.is_directory();
            } catch (const std::filesystem::__cxx11::filesystem_error &err) {
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
    //qDebug() << "generateFileList() - " << entryVec.size() << " items, time: " << t.elapsed();
}

void DirectoryManager::sortFileList() {
    QElapsedTimer t;
    t.start();
    sort(entryVec.begin(), entryVec.end(), std::bind(compareFunction(), this, std::placeholders::_1, std::placeholders::_2));
    //qDebug() << "sortFileList() - " << entryVec.size() << " items, time: " << t.elapsed();
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

SortingMode DirectoryManager::sortingMode() {
    return mSortingMode;
}

// fs watcher events

void DirectoryManager::onFileRemovedExternal(QString fileName) {
    if(!contains(fileName))
        return;
    int index = indexOf(fileName);
    entryVec.erase(entryVec.begin() + index);
    emit fileRemoved(fileName, index);
}

void DirectoryManager::onFileAddedExternal(QString fileName) {
    QString fullPath = fullFilePath(fileName);
    if(!this->isSupportedFile(fullPath))
        return;
    if(this->contains(fileName))
        return;
    std::filesystem::directory_entry stdEntry(toStdString(fullPath));
    Entry entry(fileName, stdEntry.file_size(), stdEntry.last_write_time(), stdEntry.is_directory());
    insert_sorted(entryVec, entry, std::bind(compareFunction(), this, std::placeholders::_1, std::placeholders::_2));
    emit fileAdded(fileName);
    return;
}

void DirectoryManager::onFileRenamedExternal(QString oldFile, QString newFile) {
    if(!contains(oldFile)) {
        // insert if it passes regex check
        onFileAddedExternal(newFile);
        return;
    }
    // remove old one
    int index = indexOf(oldFile);
    entryVec.erase(entryVec.begin() + index);
    // insert
    QString fullPath = fullFilePath(newFile);
    std::filesystem::directory_entry stdEntry(toStdString(fullPath));
    Entry entry(newFile, stdEntry.file_size(), stdEntry.last_write_time(), stdEntry.is_directory());
    insert_sorted(entryVec, entry, std::bind(compareFunction(), this, std::placeholders::_1, std::placeholders::_2));
    emit fileRenamed(oldFile, index, newFile, indexOf(newFile));
}

void DirectoryManager::onFileModifiedExternal(QString fileName) {
    if(!contains(fileName))
        return;
    QString fullPath = fullFilePath(fileName);
    std::filesystem::directory_entry stdEntry(toStdString(fullPath));
    int index = indexOf(fileName);
    if(entryVec.at(index).modifyTime != stdEntry.last_write_time())
        entryVec.at(index).modifyTime = stdEntry.last_write_time();
    emit fileModified(fileName);
}

bool DirectoryManager::forceInsert(QString fileName) {
    QString fullPath = fullFilePath(fileName);
    if(!this->isFile(fullPath) || contains(fileName))
        return false;
    std::filesystem::directory_entry stdEntry(toStdString(fullPath));
    Entry entry(fileName, stdEntry.file_size(), stdEntry.last_write_time(), stdEntry.is_directory());
    insert_sorted(entryVec, entry, std::bind(compareFunction(), this, std::placeholders::_1, std::placeholders::_2));
    emit fileAdded(fileName);
    return true;
}
