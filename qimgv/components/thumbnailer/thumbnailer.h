#pragma once

#include <QThreadPool>
#include <QtConcurrent>
#include "components/directorymanager/directorymanager.h"
#include "components/thumbnailer/thumbnailerrunnable.h"
#include "components/cache/thumbnailcache.h"
#include "components/cache/cache.h"
#include "settings.h"

class Thumbnailer : public QObject
{
    Q_OBJECT
public:
    explicit Thumbnailer(DirectoryManager *_dm);
    static std::shared_ptr<Thumbnail> getThumbnail(QString filePath, int size);
    void clearTasks();

public slots:
    void generateThumbnails(QList<int> indexes, int size, bool crop, bool force);

private:
    ThumbnailCache *cache;
    QThreadPool *pool;
    void startThumbnailerThread(QString filePath, int size, bool crop, bool force);
    DirectoryManager *dm;
    QMultiMap<QString, int> runningTasks;

private slots:
    void onTaskStart(QString path, int size);
    void onTaskEnd(std::shared_ptr<Thumbnail> thumbnail, QString path);

signals:
    void thumbnailReady(std::shared_ptr<Thumbnail>);
};
