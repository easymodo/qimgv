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
#include "thumbnailer.h"

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
    Image *current;

public slots:
    void generateThumbnailFor(int pos, long thumbnailId);

private:
    DirectoryManager *dm;
    ImageCache *cache;
    QMutex mutex, mutex2;
    void freeAt(int);
    int loadTarget, preloadTarget, time;
    LoadHelper *worker;
    QThread *loadThread;
    QTimer *loadTimer, *preloadTimer;

    void freeAll();
    bool isRelevant(int pos);

    const int LOAD_DELAY = 0;
signals:
    void loadStarted();
    void loadFinished(Image*, int pos);
    void thumbnailReady(long, Thumbnail*);
    void startLoad();
    void startPreload();

private slots:
    bool setLoadTarget(int);
    void lock();
    void unlock();
    void readSettings();
    void doLoad(int pos);
    void doPreload();
    void onLoadFinished(int);
    void onLoadTimeout();
    void onPreloadTimeout();
    void freeAuto();
};

#endif // NEWLOADER_H
