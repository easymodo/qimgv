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

class DirectoryManager;

typedef bool (DirectoryManager::*CompareFunction)(const FSEntry &e1, const FSEntry &e2) const;

class DirectoryManager : public QObject {
    Q_OBJECT
public:
    DirectoryManager();
    // ignored if the same dir is already opened
    bool setDirectory(QString);
    QString directoryPath() const;
    int indexOfFile(QString filePath) const;
    int indexOfDir(QString dirPath) const;
    QString filePathAt(int index) const;
    bool removeFile(QString filePath, bool trash);
    unsigned long fileCount() const;
    unsigned long dirCount() const;
    bool isSupportedFile(QString filePath) const;
    bool isEmpty() const;
    bool containsFile(QString filePath) const;
    QString fileNameAt(int index) const;
    QString prevOfFile(QString filePath) const;
    QString nextOfFile(QString filePath) const;
    void sortEntryLists();
    QDateTime lastModified(QString filePath) const;

    QString firstFile() const;
    QString lastFile() const;
    void setSortingMode(SortingMode mode);
    SortingMode sortingMode() const;
    bool forceInsertFile(QString filePath);
    bool isFile(QString path) const;

    unsigned long totalCount() const;
    bool containsDir(QString dirPath) const;
    const FSEntry &fileEntryAt(int index) const;
    QString dirPathAt(int index) const;
    QString dirNameAt(int index) const;
private:
    QRegularExpression regex;
    QCollator collator;
    std::vector<FSEntry> fileEntryVec, dirEntryVec;
    const FSEntry defaultEntry;

    DirectoryWatcher* watcher;
    void readSettings();
    SortingMode mSortingMode;
    void loadEntryList(QString directoryPath, bool recursive);

    bool moveToTrash(QString file);
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
};
