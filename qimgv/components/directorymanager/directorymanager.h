#pragma once

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

#include "settings.h"
#include "watchers/directorywatcher.h"
#include "utils/stuff.h"
#include "sourcecontainers/fsentry.h"

#ifdef Q_OS_WIN32
#include "windows.h"
#endif

enum FileListSource { // rename? wip
    SOURCE_DIRECTORY,
    SOURCE_DIRECTORY_RECURSIVE,
    SOURCE_LIST
};

class DirectoryManager;

typedef bool (DirectoryManager::*CompareFunction)(const FSEntry &e1, const FSEntry &e2) const;

//TODO: rename? EntrySomething?

class DirectoryManager : public QObject {
    Q_OBJECT
public:
    DirectoryManager();
    // ignored if the same dir is already opened
    bool setDirectory(QString);
    bool setDirectoryRecursive(QString);
    QString directoryPath() const;
    int indexOfFile(QString filePath) const;
    int indexOfDir(QString dirPath) const;
    QString filePathAt(int index) const;
    unsigned long fileCount() const;
    unsigned long dirCount() const;
    inline bool isSupportedFile(QString filePath) const;
    bool isEmpty() const;
    bool containsFile(QString filePath) const;
    QString fileNameAt(int index) const;
    QString prevOfFile(QString filePath) const;
    QString nextOfFile(QString filePath) const;
    QString prevOfDir(QString filePath) const;
    QString nextOfDir(QString filePath) const;
    void sortEntryLists();
    QDateTime lastModified(QString filePath) const;

    QString firstFile() const;
    QString lastFile() const;
    void setSortingMode(SortingMode mode);
    SortingMode sortingMode() const;
    bool isFile(QString path) const;
    bool isDir(QString path) const;

    unsigned long totalCount() const;
    bool containsDir(QString dirPath) const;
    const FSEntry &fileEntryAt(int index) const;
    QString dirPathAt(int index) const;
    QString dirNameAt(int index) const;
    bool fileWatcherActive();

    bool insertFileEntry(const QString &filePath);
    bool forceInsertFileEntry(const QString &filePath);
    void removeFileEntry(const QString &filePath);
    void updateFileEntry(const QString &filePath);
    void renameFileEntry(const QString &oldFilePath, const QString &newName);

    bool insertDirEntry(const QString &dirPath);
    //bool forceInsertDirEntry(const QString &dirPath);
    void removeDirEntry(const QString &dirPath);
    //void updateDirEntry(const QString &dirPath);
    void renameDirEntry(const QString &oldDirPath, const QString &newName);

    FileListSource source() const;

    QStringList fileList() const;

private:
    QRegularExpression regex;
    QCollator collator;
    std::vector<FSEntry> fileEntryVec, dirEntryVec;
    const FSEntry defaultEntry;
    QString mDirectoryPath;

    DirectoryWatcher* watcher;
    void readSettings();
    SortingMode mSortingMode;
    FileListSource mListSource;
    void loadEntryList(QString directoryPath, bool recursive);

    bool path_entry_compare(const FSEntry &e1, const FSEntry &e2) const;
    bool path_entry_compare_reverse(const FSEntry &e1, const FSEntry &e2) const;
    bool name_entry_compare(const FSEntry &e1, const FSEntry &e2) const;
    bool name_entry_compare_reverse(const FSEntry &e1, const FSEntry &e2) const;
    bool date_entry_compare(const FSEntry &e1, const FSEntry &e2) const;
    bool date_entry_compare_reverse(const FSEntry &e1, const FSEntry &e2) const;
    CompareFunction compareFunction();
    bool size_entry_compare(const FSEntry &e1, const FSEntry &e2) const;
    bool size_entry_compare_reverse(const FSEntry &e1, const FSEntry &e2) const;
    void startFileWatcher(QString directoryPath);
    void stopFileWatcher();

    void addEntriesFromDirectory(std::vector<FSEntry> &entryVec, QString directoryPath);
    void addEntriesFromDirectoryRecursive(std::vector<FSEntry> &entryVec, QString directoryPath);
    bool checkFileRange(int index) const;
    bool checkDirRange(int index) const;

private slots:
    void onFileAddedExternal(QString fileName);
    void onFileRemovedExternal(QString fileName);
    void onFileModifiedExternal(QString fileName);
    void onFileRenamedExternal(QString oldFileName, QString newFileName);

signals:
    void loaded(const QString &path);
    void sortingChanged();
    void fileRemoved(QString filePath, int);
    void fileModified(QString filePath);
    void fileAdded(QString filePath);
    void fileRenamed(QString fromPath, int indexFrom, QString toPath, int indexTo);

    void dirRemoved(QString dirPath, int);
    void dirAdded(QString dirPath);
    void dirRenamed(QString fromPath, int indexFrom, QString toPath, int indexTo);
};
