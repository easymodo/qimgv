#ifndef IMAGE_H
#define IMAGE_H

#include "../lib/imagelib.h"
#include "../lib/stuff.h"
#include "../fileinfo.h"
#include "../settings.h"
#include <QObject>
#include <QString>
#include <QIODevice>
#include <QDebug>
#include <QPixmap>
#include <QPixmapCache>
#include <QThread>
#include <QMutex>

class Image : public QObject
{
    Q_OBJECT
public:
    virtual QPixmap* getPixmap() = 0;
    virtual const QImage* getImage() = 0;
    fileType getType();
    virtual void load() = 0;
    QString getPath();
    virtual int height() = 0;
    virtual int width() = 0;
    virtual QSize size() = 0;
    bool isLoaded();
    virtual QPixmap* generateThumbnail() = 0;
    void attachInfo(FileInfo*);
    FileInfo* getInfo();
    void safeDeleteSelf();

    virtual void crop(QRect newRect) = 0;
    virtual void rotate(int grad) = 0;

    virtual void lock();
    virtual void unlock();

protected:
    bool loaded;
    QString path;
    QSize resolution;
    QMutex mutex;
    FileInfo* info;

signals:

public slots:
    virtual void save() = 0;
    virtual void save(QString destinationPath) = 0;
};

#endif // IMAGE_H
