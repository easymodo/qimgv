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
    }

    ~CacheObject() {
        if(img)
            img->safeDeleteSelf();
        delete thumbnail;
        delete info;
    }
    Thumbnail* getThumbnail() {
        if(!thumbnail) {
            qDebug() << "!returning empty thumbnail";
        }
        return const_cast<Thumbnail*>(thumbnail);
    }
    FileInfo* getInfo() {
        if(img)
            return img->getInfo();
    }
    bool isLoaded() {
        if(img==NULL) {
            return false;
        } else {
            return true;
        }
    }
    void unload() {
        if(img) {
            img->safeDeleteSelf();
            img = NULL;
            info = NULL;
        }
    }
    void setImage(Image* _img) {
        img = _img;
        info = img->getInfo();
    }
    void setThumbnail(Thumbnail* _thumbnail) {
        if(thumbnail && _thumbnail) {
            delete thumbnail;
        }
        thumbnail = _thumbnail;
    }
    Image* image() {
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

    // clears cache; then loads supplied file list
    // should be called on every directory change/sort
    void init(QString dir, QStringList list);
    void unloadAll();
    void unloadAt(int pos);
    Image *imageAt(unsigned int pos);
    Thumbnail *thumbnailAt(int pos) const;
    int length() const;
    QString currentDirectory();
    bool isLoaded(int pos);
    int currentlyLoadedCount();
    void setImage(Image *img, int pos);
    void setThumbnail(Thumbnail *thumb, int pos);

private:
    QList<CacheObject*> *cachedImages;
    uint maxCacheSize;
    QString dir;
    QMutex mutex;

    void lock();
    void unlock();
    void readSettings();

private slots:
    void applySettings();

signals:
    void initialized(int count);
};

#endif // IMAGECACHE_H
