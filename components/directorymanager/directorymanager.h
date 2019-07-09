#ifndef DIRECTORYMANAGER_H
#define DIRECTORYMANAGER_H

#include <QObject>
#include <QMimeDatabase>
#include <QStandardPaths>
#include <algorithm>
#include <vector>
#include <QCollator>
#include <QElapsedTimer>
#include <QUrl>
#include <QFile>
#include <QString>
#include <QSize>
#include <QDebug>
#include <QFileInfo>
#include <QMessageBox>
#include <QImageReader>
#include <QRegularExpression>

#include <string>
#include <iostream>
#include <filesystem>
#include <algorithm>
//#include <experimental/filesystem>

#include "settings.h"
#include "sourcecontainers/documentinfo.h"

#ifdef Q_OS_WIN32
#include "windows.h"
#endif

struct Entry {
    Entry( QString _path, std::uintmax_t _size, std::filesystem::file_time_type _modifyTime, bool _isDirectory)
        : path(_path),
          size(_size),
          modifyTime(_modifyTime),
          isDirectory(_isDirectory)
    {
    }
    Entry( QString _path, std::uintmax_t _size, bool _isDirectory)
        : path(_path),
          size(_size),
          isDirectory(_isDirectory)
    {
    }
    Entry( QString _path, bool _isDirectory)
        : path(_path),
          isDirectory(_isDirectory)
    {
    }
    bool operator==(const QString &anotherPath) const {
        return this->path == anotherPath;
    }
    QString path;
    std::uintmax_t size;
    std::filesystem::file_time_type modifyTime;
    //std::time_t modifyTime;
    bool isDirectory;
};

class DirectoryManager : public QObject
{
    Q_OBJECT
public:
    DirectoryManager();
    // ignored if the same dir is already opened
    void setDirectory(QString);
    // returns index in file list
    // -1 if not found
    int indexOf(QString fileName) const;
    QString absolutePath() const;
    QString filePathAt(int index) const;
    QString fullFilePath(QString fileName) const;
    bool removeFile(QString fileName, bool trash);
    unsigned long fileCount() const;
    bool isImage(QString filePath) const;
    bool isEmpty() const;
    bool contains(QString fileName) const;
    bool checkRange(int index) const;
    bool copyTo(QString destDirectory, QString fileName);
    QString fileNameAt(int index) const;
    QString prevOf(QString fileName) const;
    QString nextOf(QString fileName) const;
    bool isDirectory(QString path) const;
    void sortFileList(SortingMode mode);
    QDateTime lastModified(QString fileName) const;

    QString first();
    QString last();
private:
    QString currentPath;
    QString filter;
    QStringList mimeFilter, nameFilter;
    QRegularExpression regexpFilter;
    QCollator collator;
    std::vector<Entry> entryVec;

    void readSettings();
//    WatcherWindows watcher;
    QMimeDatabase mimeDb;
    bool quickFormatDetection;

    void generateFileList();
    //void generateFileListQuick();
    //void generateFileListDeep();

    void onFileAddedExternal(QString filename);
    void onFileRemovedExternal(QString);
    void onFileModifiedExternal(QString fileName);
    void onFileRenamedExternal(QString oldFile, QString newFile);
    void moveToTrash(QString file);
signals:
    void directoryChanged(const QString &path);
    void directorySortingChanged();
    void fileRemoved(QString, int);
    void fileModified(QString);
    void fileAdded(QString);
    void fileRenamed(QString from, QString to);
};

#endif // DIRECTORYMANAGER_H
