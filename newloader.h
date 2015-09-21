#ifndef NEWLOADER_H
#define NEWLOADER_H

#include <QObject>
#include "directorymanager.h"
#include "imagecache.h"
#include "settings.h"
#include "sourceContainers/imagestatic.h"
#include <QtConcurrent>
#include <time.h>
#include <QMutex>
#include <QVector>
#include "loadhelper.h"

class NewLoader : public QObject
{
    Q_OBJECT
public:
    explicit NewLoader(DirectoryManager *);
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
    int loadTarget, current;
    LoadHelper *worker;
    QThread *loadThread;
    QTimer *loadTimer;

signals:
    void loadStarted();
    void loadFinished(Image*, int pos);
    void startPreload(int);
    void thumbnailReady(int, const Thumbnail*);
    void startLoad();

private slots:
    void setLoadTarget(int);
    void lock();
    void unlock();
    void readSettings();
    void preload(int pos);
    void doLoad(int pos);
    void doPreload(int pos);
    void onLoadFinished(int);
    void onLoadTimeout();
};

#endif // NEWLOADER_H
