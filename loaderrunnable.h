#ifndef LOADERRUNNABLE_H
#define LOADERRUNNABLE_H

#include <QObject>
#include <QRunnable>
#include "imagefactory.h"

class LoaderRunnable: public QObject, public QRunnable
{
    Q_OBJECT
public:
    LoaderRunnable(QString _path, int _index, QThread *_mainThread);
    void run();
private:
    int index;
    QString path;
    QThread *mainThread;
signals:
    void finished(Image*, int);
};

#endif // LOADERRUNNABLE_H
