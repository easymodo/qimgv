#ifndef IMAGELOADER_H
#define IMAGELOADER_H

#include <QObject>
#include "directorymanager.h"
#include "imagecache.h"
#include "settings.h"
#include "image.h"
#include <QtConcurrent>
#include <time.h>
#include <QMutex>
#include <QVector>

class ImageLoader : public QObject
{
    Q_OBJECT
public:
    explicit ImageLoader(DirectoryManager *);
    void open(QString path);
    void load(int pos);
    void loadNext();
    void loadPrev();
    void preload(FileInfo* path);

private:
    DirectoryManager *dm;
    ImageCache *cache;
    QMutex mutex, mutex2;
    void lock();
    void unlock();

signals:
    void loadStarted();
    void loadFinished(Image*);
    void startPreload();

private slots:
    void readSettings();
    void load_thread(Image* image);
    void preload_thread(Image*);
};

#endif // IMAGELOADER_H
