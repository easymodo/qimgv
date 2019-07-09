#include "directorymanager.h"
#include "watchers/directorywatcher.h"

#include <QThread>

namespace fs = std::filesystem;

DirectoryManager::DirectoryManager() {
    currentPath = "";
    regexpFilter.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
    collator.setNumericMode(true);
    readSettings();
}

QList<QByteArray> supportedFormats() {
    QList<QByteArray> formats = QImageReader::supportedImageFormats();
    // looks like performance is fine with QRegularExpression
    /*formats.removeOne("*.bw");
    formats.removeOne("*.arw");
    formats.removeOne("*.crw");
    formats.removeOne("*.dng");
    formats.removeOne("*.eps");
    formats.removeOne("*.epsf");
    formats.removeOne("*.epsi");
    formats.removeOne("*.nef");
    formats.removeOne("*.ora");
    formats.removeOne("*.pbm");
    formats.removeOne("*.pcx");
    formats.removeOne("*.pgm");
    formats.removeOne("*.pic");
    formats.removeOne("*.ppm");
    formats.removeOne("*.psd");
    formats.removeOne("*.raf");
    formats.removeOne("*.ras");
    formats.removeOne("*.sgi");
    formats.removeOne("*.wbmp");
    formats.removeOne("*.xbm");
    formats.removeOne("*.exr");
    formats.removeOne("*.icns");
    formats.removeOne("*.mng");
    formats.removeOne("*.rgb");
    formats.removeOne("*.rgba");*/
    return formats;
}

QStringList supportedFormatsFilter() {
    QStringList filters;
    QList<QByteArray> formats = supportedFormats();
    for(int i = 0; i < formats.count(); i++) {
        filters << "*." + QString(formats.at(i));
    }
    return filters;
}

QString supportedFormatsRegex() {
    QString filter;
    QList<QByteArray> formats = supportedFormats();
    filter.append(".*\.(");
    for(int i = 0; i < formats.count(); i++) {
        filter.append(QString(formats.at(i)) + "|");
    }
    // webm here
    filter.chop(1);
    filter.append(")$");
    return filter;
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

bool DirectoryManager::entryCompareString(Entry &e, QString path) {
    return (e.path == path);
}

CompareFunction DirectoryManager::compareFunction() {
    CompareFunction cmpFn;
    if(sortingMode == SortingMode::NAME_ASC)
        cmpFn = &DirectoryManager::name_entry_compare;
    if(sortingMode == SortingMode::NAME_DESC)
        cmpFn = &DirectoryManager::name_entry_compare_reverse;
    if(sortingMode == SortingMode::DATE_ASC)
        cmpFn = &DirectoryManager::date_entry_compare;
    if(sortingMode == SortingMode::DATE_DESC)
        cmpFn = &DirectoryManager::date_entry_compare_reverse;
    return cmpFn;
}

// ##############################################################
// ####################### PUBLIC METHODS #######################
// ##############################################################

void DirectoryManager::readSettings() {
    SortingMode newMode = settings->sortingMode();
    if(newMode != sortingMode) {
        sortingMode = newMode;
        sortFileList();
    }
    mimeFilter = settings->supportedMimeTypes();
    filter = supportedFormatsRegex();
    regexpFilter.setPattern(filter);
}

bool DirectoryManager::setDirectory(QString path) {
    if(path.isEmpty()) {
        qDebug() << "[DirectoryManager] Error - path is empty.";
        return false;
    }
    if(!std::filesystem::exists(path.toStdString())) {
        qDebug() << "[DirectoryManager] Error - path does not exist.";
        return false;
    }
    if(!std::filesystem::is_directory(path.toStdString())) {
        qDebug() << "[DirectoryManager] Error - path is not a directory.";
        return false;
    }

    qDebug() << "setDirectory() start";
    QElapsedTimer t;
    t.start();
    currentPath = path;
    generateFileList();
    sortFileList();
    emit directoryChanged(path);

    DirectoryWatcher* watcher = DirectoryWatcher::newInstance();
    watcher->setWatchPath(path);
    watcher->observe();
    connect(watcher, &DirectoryWatcher::observingStarted, this, [] () {
        qDebug() << "observing started";
    });

    connect(watcher, &DirectoryWatcher::observingStopped, this, [watcher] () {
        qDebug() << "observing stopped";
    });

    connect(watcher, &DirectoryWatcher::fileCreated, this, [this] (const QString& filename) {
        qDebug() << "[w] file created" << filename;
        onFileAddedExternal(filename);
    });

    connect(watcher, &DirectoryWatcher::fileDeleted, this, [this] (const QString& filename) {
        qDebug() << "[w] file deleted" << filename;
        onFileRemovedExternal(filename);
    });

    connect(watcher, &DirectoryWatcher::fileModified, this, [this] (const QString& filename) {
        qDebug() << "[w] file modified" << filename;
        onFileModifiedExternal(filename);
    });

    connect(watcher, &DirectoryWatcher::fileRenamed, this, [this] (const QString& file1, const QString& file2) {
        qDebug() << "[w] file renamed from" << file1 << "to" << file2;
        //if(isImage(this->absolutePath() + "/" + file2))
        //    onFileRenamedExternal(file1, file2);
    });

    qDebug() << "setDirectory() end.   (" << t.elapsed() << " ms)";
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
    bool result = false;
    /*if(!contains(fileName))
        return result;
    QString path = fullFilePath(fileName);
    QFile file(path);
    int index = mFileNameList.indexOf(fileName);
    mFileNameList.removeOne(fileName);
    if(trash) {
        moveToTrash(path);
        result = true;
    } else {
        if(file.remove())
            result = true;
    }
    emit fileRemoved(fileName, index);
    */
    return result;
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
    SHFILEOPSTRUCT fileop;
    memset( &fileop, 0, sizeof( fileop ) );
    fileop.wFunc = FO_DELETE;
    fileop.pFrom = from;
    fileop.fFlags = FOF_ALLOWUNDO | FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT;
    int rv = SHFileOperation( &fileop );
    if( 0 != rv ){
        qDebug() << rv << QString::number( rv ).toInt( 0, 8 );
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
            qDebug() << "Cant detect trash folder";
        TrashPathInfo = TrashPath + "/info";
        TrashPathFiles = TrashPath + "/files";
        if( !QDir( TrashPathInfo ).exists() || !QDir( TrashPathFiles ).exists() )
            qDebug() << "Trash doesnt looks like FreeDesktop.org Trash specification";
        TrashInitialized = true;
    }
    QFileInfo original( file );
    if( !original.exists() )
        qDebug() << "File doesnt exists, cant move to trash";
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
    if(!std::filesystem::exists(path.toStdString()))
        return false;
    if(!std::filesystem::is_regular_file(path.toStdString()))
        return false;
    if(!regexpFilter.match(path).hasMatch())
        return false;
    return true;
}

bool DirectoryManager::isEmpty() const {
    return entryVec.empty();
}

bool DirectoryManager::contains(QString fileName) const {
    return ( std::find(entryVec.begin(), entryVec.end(), fileName) != entryVec.end() );
}

// ##############################################################
// ###################### PRIVATE METHODS #######################
// ##############################################################
void DirectoryManager::generateFileList() {
    entryVec.clear();
    QElapsedTimer t;
    t.start();
    QRegularExpressionMatch match;
    for (const auto & entry : fs::directory_iterator(currentPath.toStdString())) {
        QString name = QString::fromStdString(entry.path().filename());
        match = regexpFilter.match(name);
        if(match.hasMatch()) {
            entryVec.emplace_back(Entry(name, entry.file_size(), entry.last_write_time(), entry.is_directory()));
            //entryVec.emplace_back(Entry(name, entry.is_directory()));
        }
    }
    qDebug() << "generateFileList() - " << entryVec.size() << " items, time: " << t.elapsed();
}

void DirectoryManager::sortFileList() {
    QElapsedTimer t;
    t.start();
    sort(entryVec.begin(), entryVec.end(), std::bind(compareFunction(), this, std::placeholders::_1, std::placeholders::_2));
    qDebug() << "sortFileList() - " << entryVec.size() << " items, time: " << t.elapsed();
}

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
    std::filesystem::directory_entry stdEntry(fullPath.toStdString());
    Entry entry(fileName, stdEntry.file_size(), stdEntry.last_write_time(), stdEntry.is_directory());
    insert_sorted(entryVec, entry, std::bind(compareFunction(), this, std::placeholders::_1, std::placeholders::_2));
    emit fileAdded(fileName);
}

void DirectoryManager::onFileRenamedExternal(QString oldFile, QString newFile) {
    /*int index = mFileNameList.indexOf(oldFile);
    generateFileList(settings->sortingMode());
    if(contains(newFile)) {
        emit fileRenamed(oldFile, newFile);
    } else {
        emit fileRemoved(oldFile, index);
    }
    */
}

void DirectoryManager::onFileModifiedExternal(QString fileName) {
    /*if(contains(fileName)) { // file changed
        qDebug() << "fileChange: " << fileName;
        emit fileModified(fileName);
    } else { // file added?
        onFileAddedExternal(fileName);
    }
    */
}
