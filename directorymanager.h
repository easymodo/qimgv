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

class DirectoryManager : public QObject
{
    Q_OBJECT
public:
    DirectoryManager();

    QDir currentDir;
    int currentPos;
    QStringList fileNameList;
    QStringList mimeFilters, extensionFilters;

    void readSettings();

    // changes current file position
    // changes current directory if needed
    void setFile(QString path);

    // ignored if the same dir is already opened
    void setCurrentDir(QString);

    // returns true on success
    bool setCurrentPos(int pos);
    QStringList fileList();
    QString currentDirectory();
    QString currentFileName();
    QString currentFilePath();
    QString nextFilePath();
    QString prevFilePath();
    QString filePathAt(int pos);
    int currentFilePos();
    int nextPos();
    int prevPos();
    int fileCount();
    int peekNext(int offset);
    int peekPrev(int offset);
    bool existsInCurrentDir(QString file);
    bool isImage(QString filePath);
    bool containsImages();

public slots:
    void applySettingsChanges();

private:
    QString startDir;
    bool infiniteScrolling;
    QMimeDatabase mimeDb;
    bool quickFormatDetection;

    void changePath(QString path);
    FileInfo* loadInfo(QString path);
    void generateFileList();
    void generateFileListQuick();
    void generateFileListDeep();

signals:
    void directoryChanged(const QString &path);
    void directorySortingChanged();
};

#endif // DIRECTORYMANAGER_H
