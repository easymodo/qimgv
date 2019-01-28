#include "directorymanager.h"
#include "watchers/directorywatcher.h"

#include <QThread>

DirectoryManager::DirectoryManager() : quickFormatDetection(true)
{
    readSettings();
}

// ##############################################################
// ####################### PUBLIC METHODS #######################
// ##############################################################

void DirectoryManager::readSettings() {
    mimeFilter = settings->supportedMimeTypes();
    nameFilter = settings->supportedFormats();
    // looks like >40 filters is a bit too much for QDir
    // removing some exotic formats from the filter speeds things up by ~2x
    // TODO: implement lazy directory crawling instead (gwenview does this)
    //qDebug() << nameFilter.count() << nameFilter;
    QElapsedTimer t;
    t.start();
    nameFilter.removeOne("*.bw");
    nameFilter.removeOne("*.arw");
    nameFilter.removeOne("*.crw");
    nameFilter.removeOne("*.dng");
    nameFilter.removeOne("*.eps");
    nameFilter.removeOne("*.epsf");
    nameFilter.removeOne("*.epsi");
    nameFilter.removeOne("*.nef");
    nameFilter.removeOne("*.ora");
    nameFilter.removeOne("*.pbm");
    nameFilter.removeOne("*.pcx");
    nameFilter.removeOne("*.pgm");
    nameFilter.removeOne("*.pic");
    nameFilter.removeOne("*.ppm");
    nameFilter.removeOne("*.psd");
    nameFilter.removeOne("*.raf");
    nameFilter.removeOne("*.ras");
    nameFilter.removeOne("*.sgi");
    nameFilter.removeOne("*.wbmp");
    nameFilter.removeOne("*.xbm");
    nameFilter.removeOne("*.exr");
    nameFilter.removeOne("*.icns");
    nameFilter.removeOne("*.mng");
    nameFilter.removeOne("*.rgb");
    nameFilter.removeOne("*.rgba");
    currentDir.setNameFilters(nameFilter);
}

void DirectoryManager::setDirectory(QString path) {
    DirectoryWatcher* watcher = DirectoryWatcher::newInstance();
    //watcher->setFileFilters(extensionFilters);
    //watcher->setWatchPath(path);
    //watcher->observe();
    connect(watcher, &DirectoryWatcher::observingStarted, this, [] () {
        qDebug() << "observing started";
    });

    connect(watcher, &DirectoryWatcher::observingStopped, this, [watcher] () {
        qDebug() << "observing stopped";
    });

    connect(watcher, &DirectoryWatcher::fileCreated, this, [this] (const QString& filename) {
        qDebug() << "file created" << filename;
        onFileChangedExternal(filename);
    });

    connect(watcher, &DirectoryWatcher::fileDeleted, this, [this] (const QString& filename) {
        qDebug() << "file deleted" << filename;
        onFileRemovedExternal(filename);
    });

    connect(watcher, &DirectoryWatcher::fileModified, this, [this] (const QString& filename) {
        qDebug() << "file modified" << filename;
        onFileChangedExternal(filename);
    });

    connect(watcher, &DirectoryWatcher::fileRenamed, this, [watcher] (const QString& file1, const QString& file2) {
        qDebug() << "file renamed from" << file1 << "to" << file2;
    });

    if(!path.isEmpty()) {// && /* TODO: ???-> */ currentDir.exists()) {
        currentDir.setPath(path);
        generateFileList(settings->sortingMode());
        //watcher.setDir(path);
        emit directoryChanged(path);
    }
}

int DirectoryManager::indexOf(QString fileName) const {
    return mFileNameList.indexOf(fileName);
}

// full paths array
QStringList DirectoryManager::fileList() const {
    QStringList files = mFileNameList;
    QString dirPath = currentDir.absolutePath() + "/";
    for(int i = 0; i < mFileNameList.length(); i++) {
        files.replace(i, dirPath + files.at(i));
    }
    return files;
}

QString DirectoryManager::currentDirectoryPath() const {
    return currentDir.absolutePath();
}

QString DirectoryManager::filePathAt(int index) const {
    return checkRange(index) ? currentDir.absolutePath() + "/" + mFileNameList.at(index) : "";
}

QString DirectoryManager::fileNameAt(int index) const {
    return checkRange(index) ? mFileNameList.at(index) : "";
}

bool DirectoryManager::removeAt(int index, bool trash) {
    bool result = false;
    if(!checkRange(index))
        return result;

    QString path = filePathAt(index);
    QFile file(path);
    mFileNameList.removeAt(index);
    if(trash) {
        moveToTrash(path);
        result = true;
    } else {
        if(file.remove())
            result = true;
    }
    emit fileRemovedAt(index);
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
    return index >= 0 && index < mFileNameList.length();
}

bool DirectoryManager::copyTo(QString destDirectory, int index) {
    if(checkRange(index)) {
        return QFile::copy(filePathAt(index), destDirectory + "/" + fileNameAt(index));
    }
    return false;
}

int DirectoryManager::fileCount() const {
    return mFileNameList.length();
}

bool DirectoryManager::existsInCurrentDir(QString fileName) const {
    return mFileNameList.contains(fileName, Qt::CaseInsensitive);
}

bool DirectoryManager::isDirectory(QString path) const {
    QFileInfo fileInfo(path);
    return (fileInfo.isDir() && fileInfo.isReadable());
}

bool DirectoryManager::isImage(QString filePath) const {
    QFile file(filePath);
    if(file.exists()) {
        /* workaround
         * webp is detected as "audio/x-riff"
         * TODO: parse file header for this case
         */
        if(QString::compare(filePath.split('.').last(), "webp", Qt::CaseInsensitive) == 0) {
            return true;
        }
        /* end */
        QMimeType type = mimeDb.mimeTypeForFile(filePath, QMimeDatabase::MatchContent);
        if(mimeFilter.contains(type.name())) {
            return true;
        }
    }
    return false;
}

bool DirectoryManager::hasImages() const {
    return !mFileNameList.empty();
}

bool DirectoryManager::contains(QString fileName) const {
    return mFileNameList.contains(fileName);
}

// ##############################################################
// #######################  PUBLIC SLOTS  #######################
// ##############################################################

void DirectoryManager::fileChanged(const QString file) {
    qDebug() << "file changed: " << file;
}

void DirectoryManager::directoryContentsChanged(QString dirPath) {
    qDebug() << "directory changed: " << dirPath;
}

// ##############################################################
// ###################### PRIVATE METHODS #######################
// ##############################################################

// generates a sorted file list
void DirectoryManager::generateFileList(SortingMode mode) {
    // special case for natural sorting
    if(mode == SortingMode::NAME_ASC || mode == SortingMode::NAME_DESC) {
        currentDir.setSorting(QDir::NoSort);
        mFileNameList = currentDir.entryList(QDir::Files | QDir::Hidden);
        QCollator collator;
        collator.setNumericMode(true);
        if(mode == SortingMode::NAME_ASC)
            std::sort(mFileNameList.begin(), mFileNameList.end(), collator);
        else
            std::sort(mFileNameList.rbegin(), mFileNameList.rend(), collator);
        return;
    }
    // use QDir's sorting otherwise
    if(mode == SortingMode::DATE_ASC)
        currentDir.setSorting(QDir::Time);
    if(mode == SortingMode::DATE_DESC)
        currentDir.setSorting(QDir::Time | QDir::Reversed);
    if(mode == SortingMode::SIZE_ASC)
        currentDir.setSorting(QDir::Size);
    if(mode == SortingMode::SIZE_DESC)
        currentDir.setSorting(QDir::Size | QDir::Reversed);
    mFileNameList = currentDir.entryList(QDir::Files | QDir::Hidden);
}
/*
// Filter by mime type. Basically opens every file in a folder
// and checks what's inside. Very slow.
void DirectoryManager::generateFileListDeep() {
    currentDir.setNameFilters(QStringList("*"));
    mFileNameList.clear();
    QStringList unfiltered = currentDir.entryList(QDir::Files | QDir::Hidden);

    for(int i = 0; i < unfiltered.count(); i++) {
        QString filePath = currentDir.absolutePath() + "/" + unfiltered.at(i);
        if(isImage(filePath)) {
            mFileNameList.append(unfiltered.at(i));
        }
    }
}
*/

void DirectoryManager::onFileRemovedExternal(QString fileName) {
    if(mFileNameList.contains(fileName)) {
        int index = mFileNameList.indexOf(fileName);
        mFileNameList.removeOne(fileName);
        emit fileRemovedAt(index);
    }
}

void DirectoryManager::onFileChangedExternal(QString fileName) {
    if(mFileNameList.contains(fileName)) { // file changed
        qDebug() << "fileChange: " << fileName;
    } else { // file added
        qDebug() << "fileAdd: " << fileName;
        mFileNameList.append(fileName);
        //sortFileList();
        int index = mFileNameList.indexOf(fileName);
        emit fileAddedAt(index);
    }
}
