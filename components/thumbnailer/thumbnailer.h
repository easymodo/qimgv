#ifndef THUMBNAILER_H
#define THUMBNAILER_H

#include <QtConcurrent>
#include "components/directorymanager/directorymanager.h"
#include "components/thumbnailer/thumbnailerrunnable.h"
#include "components/cache/thumbnailcache.h"

class Thumbnailer : public QObject
{
    Q_OBJECT
public:
    explicit Thumbnailer(DirectoryManager *_dm);

public slots:
    void generateThumbnailFor(QList<int> indexes, int size);

private:
    ThumbnailCache *thumbnailCache;
    void startThumbnailerThread(QString filePath, int size);
    DirectoryManager *dm;
    QStringList runningTasks;

private slots:
    void onTaskStart(QString path);
    void onTaskEnd(Thumbnail*, QString path);

signals:
    void thumbnailReady(Thumbnail*);
};

#endif // THUMBNAILER_H
