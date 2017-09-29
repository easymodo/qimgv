#include "directorymanager.h"
#include "watchers/directorywatcher.h"

#include <QThread>

DirectoryManager::DirectoryManager() :
    quickFormatDetection(true)
{
    readSettings();
    connect(settings, SIGNAL(settingsChanged()),
            this, SLOT(applySettingsChanges()));
}

// ##############################################################
// ####################### PUBLIC METHODS #######################
// ##############################################################

void DirectoryManager::readSettings() {
    mimeFilters = settings->supportedMimeTypes();
    extensionFilters = settings->supportedFormats();
    applySettingsChanges();
}

void DirectoryManager::setDirectory(QString path) {
    DirectoryWatcher* watcher = DirectoryWatcher::newInstance();
    watcher->setFileFilters(extensionFilters);
    watcher->setWatchPath(path);
    watcher->observe();

    connect(watcher, &DirectoryWatcher::observingStarted, this, [] () {
        qDebug() << "observing started";
    });

    connect(watcher, &DirectoryWatcher::observingStopped, this, [watcher] () {
        qDebug() << "observing stopped";
    });

    connect(watcher, &DirectoryWatcher::fileCreated, this, [] (const QString& filename) {
        qDebug() << "file created" << filename;
    });

    connect(watcher, &DirectoryWatcher::fileDeleted, this, [] (const QString& filename) {
        qDebug() << "file deleted" << filename;
    });

    connect(watcher, &DirectoryWatcher::fileModified, this, [] (const QString& filename) {
        qDebug() << "file modified" << filename;
    });

    connect(watcher, &DirectoryWatcher::fileRenamed, this, [watcher] (const QString& file1, const QString& file2) {
        qDebug() << "file renamed from" << file1 << "to" << file2;
    });

    if(!path.isEmpty() && /* TODO: ???-> */ currentDir.exists()) {
        if(currentDir.path() != path) {
            currentDir.setPath(path);
            generateFileList();
//            watcher.setDir(path);
            emit directoryChanged(path);
        }
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

QString DirectoryManager::filePathAt(int pos) const {
    return checkRange(pos) ? currentDir.absolutePath() + "/" + mFileNameList.at(pos) : "";
}

QString DirectoryManager::fileNameAt(int pos) const {
    return checkRange(pos) ? mFileNameList.at(pos) : "";
}

bool DirectoryManager::removeAt(int pos) {
    if(checkRange(pos)) {
        QString path = filePathAt(pos);
        QFile file(path);
        // TODO: just call this method. Watcher will detect changes automatically
        if(file.remove()) {
            mFileNameList.removeAt(pos);
            return true;
        }
    }
    return false;
}

bool DirectoryManager::checkRange(int pos) const {
    return pos >= 0 && pos < mFileNameList.length();
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
        if(mimeFilters.contains(type.name())) {
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

void DirectoryManager::applySettingsChanges() {
    QDir::SortFlags flags;
    switch(settings->sortingMode()) {
        case 1:
            flags = QDir::SortFlags(QDir::Name | QDir::Reversed | QDir::IgnoreCase);
            break;
        case 2:
            flags = QDir::SortFlags(QDir::Time);
            break;
        case 3:
            flags = QDir::SortFlags(QDir::Time | QDir::Reversed);
            break;
        default:
            flags = QDir::SortFlags(QDir::Name | QDir::IgnoreCase);
            break;
    }
    if(currentDir.sorting() != flags) {
        currentDir.setSorting(flags);
        generateFileList();
        emit directorySortingChanged(); //for now, sorting dir will cause full cache reload TODO
    }
}

void DirectoryManager::fileChanged(const QString file) {
    qDebug() << "file changed: " << file;
}

void DirectoryManager::directoryContentsChanged(QString dirPath) {
    qDebug() << "directory changed: " << dirPath;
}

// ##############################################################
// ###################### PRIVATE METHODS #######################
// ##############################################################

void DirectoryManager::generateFileList() {
    switch(settings->sortingMode()) {
        case 1:
            currentDir.setSorting(QDir::Name | QDir::Reversed | QDir::IgnoreCase);
            quickFormatDetection ? generateFileListQuick() : generateFileListDeep();
            break;
        case 2:
            currentDir.setSorting(QDir::Time);
            quickFormatDetection ? generateFileListQuick() : generateFileListDeep();
            break;
        case 3:
            currentDir.setSorting(QDir::Time | QDir::Reversed);
            quickFormatDetection ? generateFileListQuick() : generateFileListDeep();
            break;
        default:
            currentDir.setSorting(QDir::Name | QDir::IgnoreCase);
            quickFormatDetection ? generateFileListQuick() : generateFileListDeep();
            break;
    }
}

// Filter by file extension, fast.
// Files with unsupported extension are ignored.
// Additionally there is a mime type check on image load (FileInfo::guessType()).
// For example an .exe wont open, but a gif with .jpg extension will still play.
void DirectoryManager::generateFileListQuick() {
    currentDir.setNameFilters(extensionFilters);
    mFileNameList = currentDir.entryList(QDir::Files | QDir::Hidden);
}

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
