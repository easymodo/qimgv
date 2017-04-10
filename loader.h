#ifndef NEWLOADER_H
#define NEWLOADER_H

#include "directorymanager.h"
#include "cache.h"
#include "settings.h"
#include <QtConcurrent>
#include <QMutex>
#include "loaderrunnable.h"
#include "thumbnailer.h"

class NewLoader : public QObject
{
    Q_OBJECT
public:
    explicit NewLoader(const DirectoryManager *);
    void open(int index);
    const ImageCache* getCache();
    void setCache(ImageCache*);
    void openBlocking(int index);
    void preload(int index);

public slots:
    void generateThumbnailFor(int index, long thumbnailId);

private:
    const DirectoryManager *dm;
    ImageCache *cache;
    QMutex mutex, mutex2;
    void freeAt(int);
    int currentIndex, preloadTarget, time;
    //QThread *loadThread;
    QTimer *loadTimer, *preloadTimer;
    // for quick access to loaded indexes
    bool isRelevant(int index);
    QList<int> tasks;

signals:
    void loadStarted();
    void loadFinished(Image*, int index);
    void thumbnailReady(long, Thumbnail*);
    void startLoad();
    void startPreload();

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
