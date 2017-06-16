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
            delete img;
    }
    void setPath(QString _path) {
        path = _path;
    }
    QString filePath() {
        return path;
    }
    ImageInfo* getInfo() {
        if(img)
            return img->info();
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
            delete img;
            //img->safeDelete();
            img = NULL;
        }
    }
    void setImage(Image* _img) {
        img = _img;
        path = img->imageInfo->filePath();
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

    void unloadAll();
    void unloadAt(int pos);
    Image *imageAt(int pos);
    int length() const;
    QString currentDirectory();
    bool isLoaded(int pos);
    QString currentlyLoadedCount();
    void setImage(Image *img, int pos);

    const QList<int> currentlyLoadedList();
private:
    QList<CacheObject*> *cachedImages;
    uint maxCacheSize;
    QString dir;
    QMutex mutex;
    // for quick access
    QList<int> loadedIndexes;

    void lock();
    void unlock();
    void readSettings();
    bool checkRange(int pos);

private slots:
    void applySettings();

public slots:
    void init(QString _dir, QStringList *fileNameList);
    void removeAt(int);

signals:
    void initialized(int count);
    void itemRemoved(int);
};

#endif // IMAGECACHE_H
