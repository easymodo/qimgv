#include "directorymanager.h"

DirectoryManager::DirectoryManager() :
    currentPos(-1),
    startDir(""),
    infiniteScrolling(false),
    quickFormatDetection(true) {
    readSettings();
    setCurrentDir(startDir);
    connect(settings, SIGNAL(settingsChanged()), this, SLOT(applySettingsChanges()));
}

// ##############################################################
// ####################### PUBLIC METHODS #######################
// ##############################################################

void DirectoryManager::readSettings() {
    infiniteScrolling = settings->infiniteScrolling();
    startDir = settings->lastDirectory();
    if(startDir.isEmpty()) {
        startDir = currentDir.homePath();
    }
    mimeFilters = settings->supportedMimeTypes();
    extensionFilters = settings->supportedFormats();
    switch(settings->sortingMode()) {
        case 1:
            currentDir.setSorting(QDir::Name | QDir::Reversed | QDir::IgnoreCase);
            break;
        case 2:
            currentDir.setSorting(QDir::Time);
            break;
        case 3:
            currentDir.setSorting(QDir::Time | QDir::Reversed);
            break;
        default:
            currentDir.setSorting(QDir::Name | QDir::IgnoreCase);
            break;
    }
    generateFileList();
}

void DirectoryManager::setFile(QString path) {
    FileInfo *info = loadInfo(path);
    setCurrentDir(info->getDirectoryPath());
    currentPos = fileNameList.indexOf(info->getFileName());
    settings->setLastFilePosition(currentPos);
}

void DirectoryManager::setCurrentDir(QString path) {
    if(currentDir.exists()) {
        if(currentDir.path() != path) {
            changePath(path);
        }
    } else changePath(path);
}

bool DirectoryManager::setCurrentPos(unsigned int pos) {
    if(containsImages() && pos <= fileCount()) {
        currentPos = pos;
        settings->setLastFilePosition(currentPos);
        return true;
    }
    qDebug() << "DirManager: invalid file position specified (" << pos << ")";
    return false;
}

// full paths array
QStringList DirectoryManager::fileList() {
    QStringList files = fileNameList;
    QString dirPath = currentDir.absolutePath() + "/";
    for(int i = 0; i < fileNameList.length(); i++) {
        files.replace(i, dirPath + files.at(i));
    }
    return files;
}

QString DirectoryManager::currentDirectory() {
    return currentDir.absolutePath();
}

QString DirectoryManager::currentFileName() {
    return fileNameList.at(currentPos);
}

QString DirectoryManager::currentFilePath() {
    QString path = currentDir.absolutePath() + "/" + fileNameList.at(currentFilePos());
    return path;
}

//returns next file's path. does not change current pos
QString DirectoryManager::nextFilePath() {
    QString path = currentDir.absolutePath() + "/" + fileNameList.at(peekNext(1));
    return path;
}

QString DirectoryManager::prevFilePath() {
    QString path = currentDir.absolutePath() + "/" + fileNameList.at(peekPrev(1));
    return path;
}

QString DirectoryManager::filePathAt(int pos) {
    if(pos < 0 || pos > fileNameList.length() - 1) {
        qDebug() << "dirManager: requested index out of range";
        return "";
    }
    QString path = currentDir.absolutePath() + "/" + fileNameList.at(pos);
    return path;
}

int DirectoryManager::currentFilePos() {
    return currentPos;
}

int DirectoryManager::nextPos() {
    currentPos = peekNext(1);
    return currentPos;
}

int DirectoryManager::prevPos() {
    currentPos = peekPrev(1);
    return currentPos;
}

int DirectoryManager::fileCount() {
    return fileNameList.length() - 1;
}

int DirectoryManager::peekNext(int offset) {
    int pos = currentPos + offset;
    if(pos < 0) {
        pos = 0;
    } else if(pos >= fileNameList.length()) {
        if(infiniteScrolling) {
            pos = 0;
        } else {
            pos = fileNameList.length() - 1;
        }
    }
    return pos;
}

int DirectoryManager::peekPrev(int offset) {
    int pos = currentPos - offset;
    if(pos < 0) {
        if(infiniteScrolling) {
            pos = fileNameList.length() - 1;
        } else {
            pos = 0;
        }
    }
    return pos;
}

bool DirectoryManager::existsInCurrentDir(QString file) {
    return fileNameList.contains(file, Qt::CaseInsensitive);
}

bool DirectoryManager::isImage(QString filePath) {
    QFile file(filePath);
    if(file.exists()) {
        QMimeType type = mimeDb.mimeTypeForFile(filePath, QMimeDatabase::MatchContent);
        if(mimeFilters.contains(type.name())) {
            return true;
        }
    }
    return false;
}

bool DirectoryManager::containsImages() {
    return !fileNameList.empty();
}

// ##############################################################
// #######################  PUBLIC SLOTS  #######################
// ##############################################################

void DirectoryManager::applySettingsChanges() {
    infiniteScrolling = settings->infiniteScrolling();
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

// ##############################################################
// ###################### PRIVATE METHODS #######################
// ##############################################################

void DirectoryManager::changePath(QString path) {
    currentDir.setPath(path);
    if(currentDir.isReadable()) {
        settings->setLastDirectory(path);
    } else {
        qDebug() << "DirManager: Invalid directory specified. Removing setting.";
        settings->setLastDirectory("");
    }
    generateFileList();
    currentPos = -1;
    emit directoryChanged(path);
}

FileInfo *DirectoryManager::loadInfo(QString path) {
    FileInfo *info = new FileInfo(path);
    return info;
}

void DirectoryManager::generateFileList() {
    quickFormatDetection ? generateFileListQuick() : generateFileListDeep();
}

// Filter by file extension, fast.
// Files with unsupported extension are ignored.
// Additionally there is a mime type check on image load (FileInfo::guessType()).
// For example an .exe wont open, but a gif with .jpg extension will still play.
void DirectoryManager::generateFileListQuick() {
    currentDir.setNameFilters(extensionFilters);
    fileNameList = currentDir.entryList();
}

// Filter by mime type. Basically opens every file in a folder
// and checks what's inside. Very slow.
void DirectoryManager::generateFileListDeep() {
    currentDir.setNameFilters(QStringList("*"));
    fileNameList.clear();
    QStringList unfiltered = currentDir.entryList();
    for(int i = 0; i < unfiltered.count(); i++) {
        QString filePath = currentDir.absolutePath() + "/" + unfiltered.at(i);
        if(isImage(filePath)) {
            fileNameList.append(unfiltered.at(i));
        }
    }
}



