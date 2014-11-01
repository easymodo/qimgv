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
    FileInfo* setFile(QString path);
    void setCurrentDir(QString);
    FileInfo* next();
    FileInfo* prev();
    FileInfo* peekPrev();
    FileInfo* peekNext();
    QDir currentDir;
    QStringList fileList;
    QStringList filters;
    int currentPos;
signals:
    void directoryChanged(const QString &path);
private:
    void changePath(QString path);
    int nextPos();
    int prevPos();
    FileInfo* loadInfo(QString path);
};

#endif // DIRECTORYMANAGER_H
