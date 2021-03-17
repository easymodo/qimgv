#pragma once

#include <QThreadPool>
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
    void clearTasks();
    void waitForDone();

public slots:
    void getThumbnailAsync(QString path, int size, bool crop, bool force);

private:
    ThumbnailCache *cache;
    QThreadPool *pool;
    void startThumbnailerThread(QString filePath, int size, bool crop, bool force);
    QMultiMap<QString, int> runningTasks;

private slots:
    void onTaskStart(QString filePath, int size);
    void onTaskEnd(std::shared_ptr<Thumbnail> thumbnail, QString filePath);

signals:
    void thumbnailReady(std::shared_ptr<Thumbnail> thumbnail, QString filePath);
};
