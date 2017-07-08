#ifndef NEWLOADER_H
#define NEWLOADER_H

#include <QtConcurrent>
#include <QMutex>
#include "settings.h"
#include "components/directorymanager/directorymanager.h"
#include "components/cache/cache2.h"
#include "components/cache/thumbnailcache.h"
#include "components/thumbnailer/thumbnailer.h"
#include "loaderrunnable.h"

class NewLoader : public QObject
{
    Q_OBJECT
public:
    explicit NewLoader(const DirectoryManager *, Cache2 *);
    void open(int index);
    const Cache2* getCache();
    void setCache(Cache2*);
    void openBlocking(int index);
    void preload(int index);

public slots:
    void generateThumbnailFor(int index, int size);

private:
    const DirectoryManager *dm;
    Cache2 *cache;
    ThumbnailCache *thumbnailCache;
    QMutex mutex, mutex2;
    void freeAt(int);
    int currentIndex, preloadTarget, time, unloadMargin;
    QTimer *loadTimer, *preloadTimer;
    // for quick access to loaded indexes
    bool isRelevant(int index);
    QList<int> tasks;

signals:
    void loadStarted();
    void loadFinished(Image*, int index);
    void thumbnailReady(int, Thumbnail*);

private slots:
    bool setLoadTarget(int);
    void lock();
    void unlock();
    void readSettings();
    void doPreload();
    void onLoadFinished(Image*, int);
    void freeAuto();
};

#endif // NEWLOADER_H
