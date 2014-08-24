#ifndef DIRECTORYMANAGER_H
#define DIRECTORYMANAGER_H

#include <QDir>
#include <QFile>
#include <QString>
#include <QSize>
#include <QDebug>
#include <QFileInfo>
#include "fileinfo.h"

class DirectoryManager
{
public:
    DirectoryManager();
    fileType setFile(QString _path);
    void changeCurrentDir(QString);
    void clearFileInfo();
    void next();
    void prev();
    FileInfo* getFile();
    QDir currentDir;
    QStringList fileList;
    QStringList filters;
    FileInfo *fileInfo;
    int currentPosition; // -1 = default (no file open)

};

#endif // DIRECTORYMANAGER_H
