#ifndef IMAGECACHE_H
#define IMAGECACHE_H

#include "image.h"
#include "settings.h"
#include <QList>
#include <QMutex>

class CacheObject {
public:
    CacheObject(QString path) : img(NULL), thumbnail(NULL) {
        img = new Image(path);
        info = new FileInfo(path);
        img->attachInfo(info);
    }
    ~CacheObject() {
        delete img;
        delete info;
    }
    void generateThumbnail() {
        if(img) {
            thumbnail = img->thumbnail();
        }
    }
    bool isLoaded() {
        return img->isLoaded();
    }
    void load() {
        img->loadImage();
    }
    void unload() {
        img->unloadImage();
    }
    Image* image() {
        return img;
    }
    FileInfo *info;
private:
    Image *img;
    QImage thumbnail;
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
private:
    QList<CacheObject*> *cachedImages;
    uint maxCacheSize;
    QMutex mutex;
    void readSettings();
private slots:
    void applySettings();
};

#endif // IMAGECACHE_H
