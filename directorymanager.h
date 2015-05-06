#ifndef DIRECTORYMANAGER_H
#define DIRECTORYMANAGER_H

#include <QObject>
#include <QDir>
#include <QFile>
#include <QString>
#include <QSize>
#include <QDebug>
#include <QFileInfo>
#include <QMessageBox>
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
    QStringList fileList;
    QStringList filters;
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
    bool isValidFile(QString path);
    bool containsFiles();
signals:
    void directoryChanged(const QString &path);
private:
    void changePath(QString path);
    FileInfo* loadInfo(QString path);
};

#endif // DIRECTORYMANAGER_H
