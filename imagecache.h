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
    CacheObject(QString _path) : img(NULL), thumbnail(NULL), info(NULL), path(_path) {
    }

    ~CacheObject() {
        delete img;
        delete info;
    }
    void generateThumbnail() {
        mutex.lock();
        delete thumbnail;
        thumbnail = getImg()->generateThumbnail();
        mutex.unlock();
    }
    const QPixmap* getThumbnail() {
        if(!thumbnail) {
            generateThumbnail();
        }
        return const_cast<const QPixmap*>(thumbnail);
    }
    const FileInfo* getInfo() {
        return const_cast<const FileInfo*>(getImg()->getInfo());
    }
    bool isLoaded() {
        if(img) {
            return img->isLoaded();
        }
        else return false;
    }
    void load() {
        getImg()->load();
        info = getImg()->getInfo();
    }
    void unload() {
        getImg()->unload();
    }
    Image* image() {
        return img;
    }
private:
    void init() {
        if(ImageLib::guessType(path) == GIF) {
            img = new ImageAnimated(path);
        } else {
            img = new ImageStatic(path);
        }
    }
    Image* getImg() {
        if(!img) {
            init();
        }
        return img;
    }
    QString path;
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
