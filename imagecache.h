#ifndef IMAGECACHE_H
#define IMAGECACHE_H

#include "imagestatic.h"
#include "imageanimated.h"
#include "sourceContainers/video.h"
#include "lib/imagelib.h"
#include "settings.h"
#include <QList>
#include <QtConcurrent>
#include <QMutex>
#include <ctime>
#include "thumbnail.h"

class CacheObject {
public:
    CacheObject(QString _path) : img(NULL), thumbnail(NULL), info(NULL), path(_path) {
        //possible slowdown here
        info = new FileInfo(path);
    }

    ~CacheObject() {
        delete img;
        delete info;
    }
    void generateThumbnail() {
        mutex.lock();
        delete thumbnail;
        thumbnail = new Thumbnail;
        thumbnail->image = getImg()->generateThumbnail();
        if(info->getType() == GIF) {
            thumbnail->name = "[gif]";
        } else if(info->getType() == VIDEO) {
                thumbnail->name = "[webm]";
        }
        if(thumbnail->image->size() == QSize(0,0)) {
            delete thumbnail->image;
            thumbnail->image = new QPixmap(":/images/res/error_no_image_100px.png");
        }
        mutex.unlock();
    }
    const Thumbnail* getThumbnail() {
        if(!thumbnail) {
            generateThumbnail();
        }
        return const_cast<const Thumbnail*>(thumbnail);
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
        mutex.lock();
        getImg()->load();
        info = getImg()->getInfo();
        mutex.unlock();
    }
    void unload() {
        mutex.lock();
        if(img) {
            img->unload();
        }
        mutex.unlock();
    }
    Image* image() {
        return img;
    }
private:
    void init() {
        if(info->getType() == GIF) {
            img = new ImageAnimated(info);
        } else if (info->getType() == VIDEO) {
            img = new Video(info);
        } else {
            img = new ImageStatic(info);
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
    Thumbnail *thumbnail;
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
    void unloadAt(int pos);
    bool isLoaded(int pos);
    int currentlyLoadedCount();
    QString currentPath();

signals:
    void initialized(int count);

public slots:
    const Thumbnail *thumbnailAt(int pos) const;
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
