#ifndef LOADERRUNNABLE_H
#define LOADERRUNNABLE_H

#include <QObject>
#include <QRunnable>
#include "utils/imagefactory.h"

class LoaderRunnable: public QObject, public QRunnable
{
    Q_OBJECT
public:
    LoaderRunnable(QString _path);
    void run();
private:
    QString path;
signals:
    void finished(Image*);
};

#endif // LOADERRUNNABLE_H
