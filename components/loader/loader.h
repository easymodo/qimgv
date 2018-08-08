#ifndef NEWLOADER_H
#define NEWLOADER_H

#include <QThreadPool>
#include <QtConcurrent>
#include "components/cache/thumbnailcache.h"
#include "loaderrunnable.h"

class Loader : public QObject {
    Q_OBJECT
public:
    explicit Loader();
    void loadBlocking(QString path);
    void loadExclusive(QString path);
    void load(QString path);

    void clearTasks();
private:
    QHash<QString, LoaderRunnable*> tasks;
    QList<QString> bufferedTasks;
    QThreadPool *pool;    
    void clearPool();
    void load(QString path, int priority);

signals:
    void loadFinished(std::shared_ptr<Image>);
    void loadFailed(QString path);

private slots:
    void onLoadFinished(std::shared_ptr<Image>, QString);
};

#endif // NEWLOADER_H
