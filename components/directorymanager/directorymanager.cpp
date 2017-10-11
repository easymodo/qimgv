#include "directorymanager.h"
#include "watchers/directorywatcher.h"

#include <QThread>

DirectoryManager::DirectoryManager() : quickFormatDetection(true)
{
    currentDir.setSorting(QDir::NoSort);
    readSettings();
}

// ##############################################################
// ####################### PUBLIC METHODS #######################
// ##############################################################

void DirectoryManager::readSettings() {
    mimeFilters = settings->supportedMimeTypes();
    extensionFilters = settings->supportedFormats();
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

    connect(watcher, &DirectoryWatcher::fileDeleted, this, [this] (const QString& filename) {
        qDebug() << "file deleted" << filename;
        onFileRemovedExternal(filename);
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

QString DirectoryManager::filePathAt(int index) const {
    return checkRange(index) ? currentDir.absolutePath() + "/" + mFileNameList.at(index) : "";
}

QString DirectoryManager::fileNameAt(int index) const {
    return checkRange(index) ? mFileNameList.at(index) : "";
}

bool DirectoryManager::removeAt(int index) {
    if(checkRange(index)) {
        QString path = filePathAt(index);
        QFile file(path);
        if(file.remove()) {
            mFileNameList.removeAt(index);
            emit fileRemovedAt(index);
            return true;
        }
    }
    return false;
}

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
    quickFormatDetection ? generateFileListQuick() : generateFileListDeep();
    sortFileList();
}

// Filter by file extension, fast.
// Files with unsupported extension are ignored.
// Additionally there is a mime type check on image load (FileInfo::guessType()).
// For example an .exe wont open, but a gif with .jpg extension will still play.
void DirectoryManager::generateFileListQuick() {
    currentDir.setNameFilters(extensionFilters);
    mFileNameList = currentDir.entryList(QDir::Files | QDir::Hidden);
}

void DirectoryManager::sortFileList() {
    QCollator collator;
    collator.setNumericMode(true);
    std::sort(mFileNameList.begin(), mFileNameList.end(), collator);
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

void DirectoryManager::onFileRemovedExternal(QString fileName) {
    if(mFileNameList.contains(fileName)) {
        int index = mFileNameList.indexOf(fileName);
        mFileNameList.removeOne(fileName);
        emit fileRemovedAt(index);
    }
}
