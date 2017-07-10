#ifndef NEWLOADER_H
#define NEWLOADER_H

#include <QtConcurrent>
#include "components/cache/thumbnailcache.h"
#include "loaderrunnable.h"

class Loader : public QObject
{
    Q_OBJECT
public:
    explicit Loader();
    void open(QString path);
    void openBlocking(QString path);

private:
    QList<QString> tasks;

signals:
    void loadFinished(Image*);

private slots:
    void onLoadFinished(Image*);
};

#endif // NEWLOADER_H
