#pragma once

#include <QThreadPool>
#include <QtConcurrent>
#include "components/thumbnailer/thumbnailerrunnable.h"
#include "components/cache/thumbnailcache.h"
#include "settings.h"

class Thumbnailer : public QObject
{
    Q_OBJECT
public:
    explicit Thumbnailer();
    ~Thumbnailer();
    static std::shared_ptr<Thumbnail> getThumbnail(QString filePath, int size);
    void clearQueuedTasks();

    void clearAllTasks();
    void setParameters(int _size, bool _crop);
public slots:
    void getThumbnailAsync(QString path, bool force);

private:
    ThumbnailCache *cache;
    QThreadPool *pool;
    void startThumbnailerThread(QString filePath, int size, bool crop, bool force);
    QList<QString> runningTasks;
    QList<QString> bufferedTasks;

    int size;
    bool crop;

    void tryToStartMoreThreads();

private slots:
    void onTaskEnd(std::shared_ptr<Thumbnail> thumbnail, QString filePath);

signals:
    void thumbnailReady(std::shared_ptr<Thumbnail> thumbnail, QString filePath);
};
