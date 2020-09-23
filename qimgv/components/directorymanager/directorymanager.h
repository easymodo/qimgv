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
    // returns index in file list
    // -1 if not found
    int indexOf(QString filePath) const;
    QString filePathAt(int index) const;
    QString fullFilePath(QString fileName) const; // todo: is this needed anymore?
    bool removeFile(QString filePath, bool trash);
    unsigned long fileCount() const;
    bool isSupportedFile(QString filePath) const;
    bool isEmpty() const;
    bool contains(QString filePath) const;
    bool checkRange(int index) const;
    QString fileNameAt(int index) const;
    QString prevOf(QString filePath) const;
    QString nextOf(QString filePath) const;
    void sortFileList();
    QDateTime lastModified(QString filePath) const;

    QString first() const;
    QString last() const;
    void setSortingMode(SortingMode mode);
    SortingMode sortingMode() const;
    bool forceInsert(QString filePath);
    bool isFile(QString path) const;

    const FSEntry &entryAt(int index) const;
private:
    QRegularExpression regex;
    QCollator collator;
    std::vector<FSEntry> entryVec;

    DirectoryWatcher* watcher;
    void readSettings();
    SortingMode mSortingMode;
    void loadFileList(QString directoryPath, bool recursive);

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

    void addFromDirectory(std::vector<FSEntry> &entryVec, QString directoryPath);
    void addFromDirectoryRecursive(std::vector<FSEntry> &entryVec, QString directoryPath);
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
