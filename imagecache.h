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
    CacheObject(QString _path) : img(NULL), path(_path) {
    }

    ~CacheObject() {
        if(img)
            img->safeDeleteSelf();
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
        }
    }
    void setImage(Image* _img) {
        img = _img;
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
    Image *imageAt(int pos);
    int length() const;
    QString currentDirectory();
    bool isLoaded(int pos);
    int currentlyLoadedCount();
    void setImage(Image *img, int pos);

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
