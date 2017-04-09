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
#include "fileinfo.h"
#include "settings.h"
#include "filesystemWatchers/watcherwindows.h"

class DirectoryManager : public QObject
{
    Q_OBJECT
public:
    DirectoryManager();

    QDir currentDir;
    QStringList mFileNameList;
    QStringList mimeFilters, extensionFilters;

    void readSettings();

    // changes current file position
    // changes current directory if needed
    void setFile(QString path);

    // ignored if the same dir is already opened
    void setDirectory(QString);

    // returns position in file list
    // -1 if not found
    int indexOf(QString);

    QStringList fileList();
    QString currentDirectory();
    QString filePathAt(int pos);
    bool removeAt(int pos);
    int fileCount();
    bool existsInCurrentDir(QString file);
    bool isImage(QString filePath);
    bool containsImages();
    bool checkRange(int pos);

public slots:
    void applySettingsChanges();

private slots:
    void fileChanged(const QString file);
    void directoryContentsChanged(QString dirPath);

private:
    WatcherWindows watcher;
    QString startDir;
    bool infiniteScrolling;
    QMimeDatabase mimeDb;
    bool quickFormatDetection;

    FileInfo* loadInfo(QString path);
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
