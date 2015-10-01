#ifndef DIRECTORYMANAGER_H
#define DIRECTORYMANAGER_H

#include <QObject>
#include <QMimeDatabase>
#include <QDir>
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
    void setFile(QString path);
    void setCurrentDir(QString);
    QDir currentDir;
    QStringList fileNameList;
    QStringList mimeFilters;
    int currentPos;
    QStringList getFileList();
    QFileInfoList getFileInfoList();
    bool existsInCurrentDir(QString file);
    QString currentFileName();
    QString currentDirectory();
    int currentFilePos();
    int nextPos();
    int prevPos();
    int lastPos();
    int peekNext(int offset);
    int peekPrev(int offset);
    void setCurrentPos(int pos);
    bool isValidFile(QString filePath);
    bool containsFiles();
    void readSettings();

    QString nextFilePath();
    QString prevFilePath();
    QString currentFilePath();
    QString filePathAt(int pos);

public slots:
    void applySettingsChanges();

signals:
    void directoryChanged(const QString &path);
    void directorySortingChanged();

private:
    void changePath(QString path);
    FileInfo* loadInfo(QString path);
    QString startDir;
    bool infiniteScrolling;
    void generateFileList();
    QMimeDatabase mimeDb;
};

#endif // DIRECTORYMANAGER_H
