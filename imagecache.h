#ifndef IMAGECACHE_H
#define IMAGECACHE_H

#include "imagestatic.h"
#include "imageanimated.h"
#include "lib/imagelib.h"
#include "settings.h"
#include <QList>
#include <QtConcurrent>
#include <QMutex>
#include <ctime>

class CacheObject {
public:
    CacheObject(QString path) : img(NULL), thumbnail(NULL), info(NULL) {
        if(ImageLib::guessType(path) == GIF) {
            img = new ImageAnimated(path);
        } else {
            img = new ImageStatic(path);
        }
    }
    ~CacheObject() {
        delete img;
        delete info;
    }
    void generateThumbnail() {
        mutex.lock();
        if(img && !thumbnail) {
            thumbnail = img->generateThumbnail();
        }
        mutex.unlock();
    }
    const QPixmap* getThumbnail() {
        if(!thumbnail) {
            generateThumbnail();
        }
        return const_cast<const QPixmap*>(thumbnail);
    }
    const FileInfo* getInfo() {
        return const_cast<const FileInfo*>(img->getInfo());
    }
    bool isLoaded() {
        return img->isLoaded();
    }
    void load() {
        img->load();
        info = img->getInfo();
    }
    void unload() {
        img->unload();
    }
    Image* image() {
        return img;
    }
private:
    FileInfo *info;
    Image *img;
    QPixmap *thumbnail;
    QMutex mutex;
};


class ImageCache : public QObject
{
    Q_OBJECT
public:
    ImageCache();
    ~ImageCache();
    void lock();
    void unlock();
    void loadAt(int pos);
    Image *imageAt(int pos);
    void init(QString dir, QStringList list);
    const FileInfo *infoAt(int pos);
    int length() const;
    void generateAllThumbnails();
    void unloadAll();
    QFuture<void> *future;
signals:
    void initialized(int count);
public slots:
    const QPixmap *thumbnailAt(int pos) const;
    QString directory();
private:
    QList<CacheObject*> *cachedImages;
    uint maxCacheSize;
    QString dir;
    QMutex mutex;
    void readSettings();
private slots:
    void applySettings();
};

#endif // IMAGECACHE_H
