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
#include "loaderrunnable.h"
#include "thumbnailer.h"

class NewLoader : public QObject
{
    Q_OBJECT
public:
    explicit NewLoader(DirectoryManager *);
    void open(int pos);
    const ImageCache* getCache();
    void setCache(ImageCache*);
    void openBlocking(int pos);
    void preload(int pos);

public slots:
    void generateThumbnailFor(int pos, long thumbnailId);

private:
    DirectoryManager *dm;
    ImageCache *cache;
    QMutex mutex, mutex2;
    void freeAt(int);
    int currentIndex, preloadTarget, time;
    LoadHelper *worker;
    //QThread *loadThread;
    QTimer *loadTimer, *preloadTimer;
    // for quick access to loaded indexes

    void freeAll();
    bool isRelevant(int pos);
    QList<int> tasks;

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
    void doPreload();
    void onLoadFinished(Image*, int);
    void freeAuto();
};

#endif // NEWLOADER_H
