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
    CacheObject(QString path) : img(NULL), thumbnail(NULL) {
        if(ImageLib::guessType(path) == GIF) {
            img = new ImageAnimated(path);
        } else {
            img = new ImageStatic(path);
        }
        info = new FileInfo(path);
        img->attachInfo(info);
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
        return const_cast<const FileInfo*>(info);
    }
    bool isLoaded() {
        return img->isLoaded();
    }
    void load() {
        img->load();
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
    void init(QStringList list);
    const FileInfo *infoAt(int pos);
    int length() const;
    void generateAllThumbnails();
    void unloadAll();
signals:
    void initialized();
public slots:
    const QPixmap *thumbnailAt(int pos) const;
private:
    QList<CacheObject*> *cachedImages;
    uint maxCacheSize;
    QMutex mutex;
    void readSettings();
private slots:
    void applySettings();
};

#endif // IMAGECACHE_H
