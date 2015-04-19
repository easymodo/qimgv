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
    void open(int pos);
    void loadNext();
    void loadPrev();
    const ImageCache* getCache();
    void setCache(ImageCache*);

private:
    DirectoryManager *dm;
    ImageCache *cache;
    QMutex mutex, mutex2;

signals:
    void loadStarted();
    void loadFinished(Image*);
    void startPreload(int);

private slots:
    void lock();
    void unlock();
    void readSettings();
    void preload(int pos);
    void load_thread(int pos);
    void preload_thread(int pos);
};

#endif // IMAGELOADER_H
