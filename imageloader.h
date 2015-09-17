#ifndef IMAGELOADER_H
#define IMAGELOADER_H

#include <QObject>
#include "directorymanager.h"
#include "imagecache.h"
#include "settings.h"
#include "sourceContainers/imagestatic.h"
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
    void openBlocking(QString path);
    void reinitCache();

public slots:
    void reinitCacheForced();
    void generateThumbnailFor(int pos);

private:
    DirectoryManager *dm;
    ImageCache *cache;
    QMutex mutex, mutex2;
    void generateThumbnailThread(int pos);
    bool reduceRam;
    void freePrev();
    void freeNext();

signals:
    void loadStarted();
    void loadFinished(Image*, int pos);
    void startPreload(int);
    void thumbnailReady(int, const Thumbnail*);

private slots:
    void lock();
    void unlock();
    void readSettings();
    void preload(int pos);
    void doLoad(int pos);
    void doPreload(int pos);
};

#endif // IMAGELOADER_H
