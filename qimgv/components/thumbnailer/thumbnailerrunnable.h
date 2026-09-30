#pragma once

#include <QRunnable>
#include <QProcess>
#include <QThread>
#include <QCryptographicHash>
#include <ctime>
#include "sourcecontainers/thumbnail.h"
#include "components/cache/thumbnailcache.h"
#include "utils/imagefactory.h"
#include "utils/imagelib.h"
#include "settings.h"
#include <memory>
#include <QImageWriter>

class ThumbnailerRunnable : public QObject, public QRunnable {
    Q_OBJECT
public:
    ThumbnailerRunnable(ThumbnailCache* _cache, QString _path, int _size, bool _crop, bool _force, QString _nameSuffix = QString());
    ~ThumbnailerRunnable();
    void run();
    // nameSuffix is appended to the displayed file name as-is (e.g. " + arw + xmp" for a grouped file)
    static std::shared_ptr<Thumbnail> generate(ThumbnailCache *cache, QString path, int size, bool crop, bool force, QString nameSuffix = QString());
private:
    static QString generateIdString(QString path, int size, bool crop);
    static std::pair<QImage*, QSize> createThumbnail(QString path, const char* format, int size, bool crop);
    static std::pair<QImage*, QSize> createVideoThumbnail(QString path, int size, bool crop);
    QString path;
    int size;
    bool crop, force;
    QString nameSuffix;
    ThumbnailCache* cache = nullptr;

signals:
    void taskStart(QString, int);
    void taskEnd(std::shared_ptr<Thumbnail>, QString);
};
