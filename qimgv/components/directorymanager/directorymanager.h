#ifndef DIRECTORYMANAGER_H
#define DIRECTORYMANAGER_H

#include <QObject>
#include <QCollator>
#include <QElapsedTimer>
#include <QString>
#include <QSize>
#include <QDebug>
#include <QDateTime>
#include <QRegularExpression>

#include <vector>
#include <string>
#include <iostream>
#include <filesystem>
#include <algorithm>
//#include <experimental/filesystem>

#include "settings.h"
#include "watchers/directorywatcher.h"
#include "utils/stuff.h"

#ifdef Q_OS_WIN32
#include "windows.h"
#endif

class DirectoryManager;
class Entry {
public:
    Entry() { }
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
    bool isDirectory;
};

typedef bool (DirectoryManager::*CompareFunction)(const Entry &e1, const Entry &e2) const;

class DirectoryManager : public QObject {
    Q_OBJECT
public:
    DirectoryManager();
    // ignored if the same dir is already opened
    bool setDirectory(QString);
    // returns index in file list
    // -1 if not found
    int indexOf(QString fileName) const;
    QString absolutePath() const;
    QString filePathAt(int index) const;
    QString fullFilePath(QString fileName) const;
    bool removeFile(QString fileName, bool trash);
    unsigned long fileCount() const;
    bool isSupportedFile(QString filePath) const;
    bool isEmpty() const;
    bool contains(QString fileName) const;
    bool checkRange(int index) const;
    bool copyTo(QString destDirectory, QString fileName);
    QString fileNameAt(int index) const;
    QString prevOf(QString fileName) const;
    QString nextOf(QString fileName) const;
    bool isDirectory(QString path) const;
    void sortFileList();
    QDateTime lastModified(QString fileName) const;

    QString first();
    QString last();
    void setSortingMode(SortingMode mode);
    SortingMode sortingMode();
    bool forceInsert(QString fileName);
    bool isFile(QString path) const;
private:
    QString currentPath;
    QString filterRegex;
    QRegularExpression regex;
    QCollator collator;
    std::vector<Entry> entryVec;

    DirectoryWatcher* watcher;
    void readSettings();
    SortingMode mSortingMode;
    void generateFileList();

    void onFileAddedExternal(QString filename);
    void onFileRemovedExternal(QString);
    void onFileModifiedExternal(QString fileName);
    void onFileRenamedExternal(QString oldFile, QString newFile);
    void moveToTrash(QString file);
    bool name_entry_compare(const Entry &e1, const Entry &e2) const;
    bool name_entry_compare_reverse(const Entry &e1, const Entry &e2) const;
    bool date_entry_compare(const Entry &e1, const Entry &e2) const;
    bool date_entry_compare_reverse(const Entry &e1, const Entry &e2) const;
    bool entryCompareString(Entry &e, QString path);
    CompareFunction compareFunction();
    bool size_entry_compare(const Entry &e1, const Entry &e2) const;
    bool size_entry_compare_reverse(const Entry &e1, const Entry &e2) const;
signals:
    void loaded(const QString &path);
    void sortingChanged();
    void fileRemoved(QString, int);
    void fileModified(QString);
    void fileAdded(QString);
    void fileRenamed(QString from, int indexFrom, QString to, int indexTo);
};

#endif // DIRECTORYMANAGER_H
