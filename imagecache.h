#ifndef IMAGECACHE_H
#define IMAGECACHE_H

#include "sourceContainers/imagestatic.h"
#include "sourceContainers/imageanimated.h"
#include "sourceContainers/video.h"
#include "sourceContainers/thumbnail.h"
#include "lib/imagelib.h"
#include "settings.h"
#include <QList>
#include <QtConcurrent>
#include <QMutex>
#include <ctime>

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
        if(!img) qDebug() << "returning null 3";
        thumbnail->image = new QPixmap();
        //thumbnail->image = getImg()->generateThumbnail();
        if(info->getType() == GIF) {
            thumbnail->label = "[gif]";
        } else if(info->getType() == VIDEO) {
                thumbnail->label = "[webm]";
        }
        if(thumbnail->image->size() == QSize(0,0)) {
            delete thumbnail->image;
            thumbnail->image = new QPixmap(":/images/res/error_no_image_100px.png");
        }
        thumbnail->name = info->getFileName();
        mutex.unlock();
    }
    const Thumbnail* getThumbnail() {
        if(!thumbnail) {
            qDebug() << "!returning empty thumbnail";
        }
        return const_cast<const Thumbnail*>(thumbnail);
    }
    const FileInfo* getInfo() {
        return const_cast<const FileInfo*>(info);
    }
    bool isLoaded() {
        if(img==NULL) {
            return false;
        } else {
            return true;
        }
    }
    void load() {
        mutex.lock();
        //getImg()->load();
        //info = getImg()->getInfo();
        mutex.unlock();
    }
    void unload() {
        if(img) {
            img->safeDeleteSelf();
            img = NULL;
        }
    }
    void setImage(Image* _img) {
        img = _img;
        info = img->getInfo();
        qDebug() << img->thread();
    }
    void setThumbnail(Thumbnail* _thumbnail) {
        if(thumbnail && _thumbnail) {
            delete thumbnail;
        }
        thumbnail = _thumbnail;
    }
    Image* image() {
        if(!img) qDebug() << "returning null 2";
        return img;
    }
private:
    void init() {

    }
    Image* getImg() {
        if(!img) {
            init();
        }
        if(!img) qDebug() << "returning null 1";
        return img;
    }
    Image *img;
    Thumbnail *thumbnail;
    FileInfo *info;
    QString path;
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
    Image *imageAt(int pos);
    void init(QString dir, QStringList list);
    int length() const;
    void unloadAll();
    QFuture<void> *future;
    void unloadAt(int pos);
    bool isLoaded(int pos);
    int currentlyLoadedCount();

    void insert(Image *img, int pos);
    void insertThumbnail(Thumbnail *thumb, int pos);
signals:
    void initialized(int count);

public slots:
    const Thumbnail *thumbnailAt(int pos) const;
    QString currentDirectory();

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
