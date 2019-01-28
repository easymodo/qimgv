#ifndef THUMBNAILER_H
#define THUMBNAILER_H

#include <QThreadPool>
#include <QtConcurrent>
#include "components/directorymanager/directorymanager.h"
#include "components/thumbnailer/thumbnailerrunnable.h"
#include "components/cache/thumbnailcache.h"
#include "settings.h"

class Thumbnailer : public QObject
{
    Q_OBJECT
public:
    explicit Thumbnailer(DirectoryManager *_dm);
    void clearTasks();

public slots:
    void generateThumbnailFor(QList<int> indexes, int size);

private:
    ThumbnailCache *thumbnailCache;
    QThreadPool *pool;
    void startThumbnailerThread(QString filePath, int size);
    DirectoryManager *dm;
    QMultiMap<QString, int> runningTasks;

private slots:
    void onTaskStart(QString path, int size);
    void onTaskEnd(std::shared_ptr<Thumbnail> thumbnail, QString path);

signals:
    void thumbnailReady(std::shared_ptr<Thumbnail>);
};

#endif // THUMBNAILER_H
