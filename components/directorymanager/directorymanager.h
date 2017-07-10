#ifndef DIRECTORYMANAGER_H
#define DIRECTORYMANAGER_H

#include <QObject>
#include <QMimeDatabase>
#include <QDir>
#include <QCollator>
#include <algorithm>
#include <vector>
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
#include "sourcecontainers/imageinfo.h"
#include "watcherwindows.h"

class DirectoryManager : public QObject
{
    Q_OBJECT
public:
    DirectoryManager();
    // ignored if the same dir is already opened
    void setDirectory(QString);
    // returns position in file list
    // -1 if not found
    int indexOf(QString fileName) const;
    QStringList fileList() const;
    QString currentDirectory() const;
    QString filePathAt(int pos) const;
    bool removeAt(int pos);
    int fileCount() const;
    bool existsInCurrentDir(QString fileName) const;
    bool isImage(QString filePath) const;
    bool hasImages() const;
    bool contains(QString fileName) const;
    bool checkRange(int pos) const;
    bool copyTo(QString destDirectory, int index);

    QString fileNameAt(int pos) const;
    bool isDirectory(QString path) const;
public slots:
    void applySettingsChanges();

private slots:
    void fileChanged(const QString file);
    void directoryContentsChanged(QString dirPath);

private:
    QDir currentDir;
    QStringList mFileNameList;
    QStringList mimeFilters, extensionFilters;

    void readSettings();
    WatcherWindows watcher;
    QMimeDatabase mimeDb;
    bool quickFormatDetection;

    void generateFileList();
    void generateFileListQuick();
    void generateFileListDeep();

signals:
    void directoryChanged(const QString &path);
    void directorySortingChanged();
    void fileRemoved(int);
    void fileChangedAt(int);
};

#endif // DIRECTORYMANAGER_H
