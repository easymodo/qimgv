#include "directorymanager.h"

DirectoryManager::DirectoryManager() :
    currentPos(-1),
    startDir(""),
    infiniteScrolling(false),
    quickFormatDetection(true)
{
    readSettings();
    setCurrentDir(startDir);
    connect(globalSettings, SIGNAL(settingsChanged()), this, SLOT(applySettingsChanges()));
}

void DirectoryManager::setCurrentDir(QString path) {
    if(currentDir.exists()) {
        if(currentDir.path() != path) {
            changePath(path);
        }
    }
    else changePath(path);
}

// Filter by mime type. Basically opens every file in a folder
// and checks what's inside. Very slow.
void DirectoryManager::generateFileList() {
    currentDir.setNameFilters(QStringList("*"));
    fileNameList.clear();
    QStringList unfiltered = currentDir.entryList();
    for(int i = 0; i < unfiltered.count(); i++) {
        QString filePath = currentDir.absolutePath()+"/"+unfiltered.at(i);
        if(isValidFile(filePath)) {
            fileNameList.append(unfiltered.at(i));
        }
    }
}

// Filter by file extension, fast.
// Files with unsupported extension are ignored.
// Additionally there is a mime type check on image load (FileInfo::guessType()).
// For example an .exe wont open, but a gif with .jpg extension will still play.
void DirectoryManager::generateFileListQuick() {
    currentDir.setNameFilters(extensionFilters);
    fileNameList = currentDir.entryList();
}

void DirectoryManager::changePath(QString path) {
    currentDir.setPath(path);
    if(currentDir.isReadable()) {
        globalSettings->setLastDirectory(path);
    } else {
        qDebug() << "DirManager: Invalid directory specified. Removing setting.";
        globalSettings->setLastDirectory("");
    }
    quickFormatDetection?generateFileListQuick():generateFileList();
    currentPos = -1;
    emit directoryChanged(path);
}

// full paths array
QStringList DirectoryManager::getFileList() {
    QStringList files = fileNameList;
    QString dirPath = currentDir.absolutePath() + "/";
    for(int i=0; i<fileNameList.length(); i++) {
        files.replace(i, dirPath+files.at(i));
    }
    return files;
}

bool DirectoryManager::existsInCurrentDir(QString file) {
    return fileNameList.contains(file, Qt::CaseInsensitive);
}

void DirectoryManager::setFile(QString path) {
    FileInfo *info = loadInfo(path);
    setCurrentDir(info->getDirectoryPath());
    currentPos = fileNameList.indexOf(info->getFileName());
    globalSettings->setLastFilePosition(currentPos);
}

void DirectoryManager::setCurrentPos(int pos) {
    currentPos = pos;
    globalSettings->setLastFilePosition(currentPos);
}

bool DirectoryManager::isValidFile(QString filePath) {
    QFile file(filePath);
    if(file.exists()) {
        QMimeType type = mimeDb.mimeTypeForFile(filePath, QMimeDatabase::MatchContent);
        if(mimeFilters.contains(type.name())) {
            return true;
        }
    }
    return false;
}

QString DirectoryManager::filePathAt(int pos) {
    if(pos<0 || pos >fileNameList.length()-1) {
        qDebug() << "dirManager: requested index out of range";
        return "";
    }
    QString path = currentDir.absolutePath()+"/"+fileNameList.at(pos);
    return path;
}

QString DirectoryManager::currentFileName() {
    return fileNameList.at(currentPos);
}

//returns next file's path. does not change current pos
QString DirectoryManager::nextFilePath() {
    QString path = currentDir.absolutePath()+"/"+fileNameList.at(peekNext(1));
    return path;
}

QString DirectoryManager::prevFilePath() {
    QString path = currentDir.absolutePath()+"/"+fileNameList.at(peekPrev(1));
    return path;
}

QString DirectoryManager::currentFilePath() {
    QString path = currentDir.absolutePath()+"/"+fileNameList.at(currentFilePos());
    return path;
}

QString DirectoryManager::currentDirectory() {
    return currentDir.absolutePath();
}

int DirectoryManager::currentFilePos() {
    return currentPos;
}

bool DirectoryManager::containsFiles() {
    return !fileNameList.empty();
}

void DirectoryManager::readSettings() {
    infiniteScrolling = globalSettings->infiniteScrolling();
    startDir = globalSettings->lastDirectory();
    if( startDir.isEmpty() ) {
        startDir = currentDir.homePath();
    }
    mimeFilters = globalSettings->supportedMimeTypes();
    extensionFilters = globalSettings->supportedFormats();
    switch(globalSettings->sortingMode()) {
        case 1: currentDir.setSorting(QDir::Name | QDir::Reversed); break;
        case 2: currentDir.setSorting(QDir::Time); break;
        case 3: currentDir.setSorting(QDir::Time | QDir::Reversed); break;
        default: currentDir.setSorting(QDir::Name); break;
    }
    generateFileList();
}

void DirectoryManager::applySettingsChanges() {
    infiniteScrolling = globalSettings->infiniteScrolling();
    QDir::SortFlags flags;
    switch(globalSettings->sortingMode()) {
        case 1: flags = QDir::SortFlags(QDir::Name | QDir::Reversed); break;
        case 2: flags = QDir::SortFlags(QDir::Time); break;
        case 3: flags = QDir::SortFlags(QDir::Time | QDir::Reversed); break;
        default: flags = QDir::SortFlags(QDir::Name); break;
    }
    if(currentDir.sorting() != flags) {
        currentDir.setSorting(flags);
        generateFileList();
        emit directorySortingChanged(); //for now, sorting dir will cause full cache reload TODO
    }
}

int DirectoryManager::nextPos() {
    currentPos = peekNext(1);
    return currentPos;
}

int DirectoryManager::prevPos() {
    currentPos = peekPrev(1);
    return currentPos;
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

int DirectoryManager::lastPos() {
    return fileNameList.length()-1;
}

FileInfo* DirectoryManager::loadInfo(QString path) {
    FileInfo *info = new FileInfo(path);
    return info;
}
