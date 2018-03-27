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
    QList<QString> bufferedTasks;
    QThreadPool *pool;

signals:
    void loadFinished(Image*);
    void loadFailed(QString path);

private slots:
    void onLoadFinished(Image*, QString);
};

#endif // NEWLOADER_H
