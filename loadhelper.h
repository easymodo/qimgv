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
    explicit LoadHelper(ImageCache *_cache, QThread *mainThread, QObject *parent = 0);


    void setTarget(int pos, QString);
    int target();
signals:
    void finished(int);

public slots:
    void doLoad();

private:
    ImageCache *cache;
    int loadTarget;
    QString path;
    QMutex mutex;
    QThread *mainThread;

};

#endif // LOADHELPER_H
