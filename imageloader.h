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
    void load(QString path);
    void load(FileInfo* file);
    void loadNext();
    void loadPrev();
    void preload(FileInfo* path);

private:
    DirectoryManager *dm;
    ImageCache *cache;
    Image* currentImg;
    QMutex mutex, mutex2;
    void lock();
    void unlock();
    Image *getCurrentImg() const;
    void setCurrentImg(Image *value);
    bool isCurrent(Image *img);
    QVector<Image*> currentJobs;
    bool loadDelayEnabled;

    bool jobAlreadyStarted(Image *img);
signals:
    void loadStarted();
    void loadFinished(Image*);
    void startPreload();

private slots:
    void readSettings();
    void preloadNearest();
    void load_thread(Image* image);
    void preload_thread(Image*);
};

#endif // IMAGELOADER_H
