#ifndef LOADHELPER_H
#define LOADHELPER_H

#include <QObject>
#include <QMutex>
#include "imagecache.h"
#include "imagefactory.h"

class LoadHelper : public QObject
{
    Q_OBJECT
public:
    explicit LoadHelper(ImageCache *_cache, QThread *mainThread);
    void setTarget(int pos, QString);
    int target();

public slots:
    void doLoad();

private:
    ImageCache *cache;
    int loadTarget;
    QString path;
    QMutex mutex;
    QThread *mainThread;

signals:
    void finished(int);

};

#endif // LOADHELPER_H
