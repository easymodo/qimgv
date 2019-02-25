#ifndef DIRECTORYMANAGER_H
#define DIRECTORYMANAGER_H

#include <QObject>
#include <QMimeDatabase>
#include <QStandardPaths>
#include <QDir>
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
#include "settings.h"
#include "sourcecontainers/documentinfo.h"

#ifdef Q_OS_WIN32
#include "windows.h"
#endif

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
    QStringList fileList() const;
    QString currentDirectoryPath() const;
    QString filePathAt(int index) const;
    QString fullFilePath(QString fileName) const;
    bool removeFile(QString fileName, bool trash);
    int fileCount() const;
    bool existsInCurrentDir(QString fileName) const;
    bool isImage(QString filePath) const;
    bool hasImages() const;
    bool contains(QString fileName) const;
    bool checkRange(int index) const;
    bool copyTo(QString destDirectory, QString fileName);
    QString fileNameAt(int index) const;
    QString prevOf(QString fileName) const;
    QString nextOf(QString fileName) const;
    bool isDirectory(QString path) const;
    void sortFileList();
    void sortFileList(SortingMode mode);
    QDateTime lastModified(QString fileName) const;

private:
    QDir currentDir;
    QStringList mFileNameList;
    QStringList mimeFilter, nameFilter;

    void readSettings();
//    WatcherWindows watcher;
    QMimeDatabase mimeDb;
    bool quickFormatDetection;

    void generateFileList(SortingMode mode);
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
