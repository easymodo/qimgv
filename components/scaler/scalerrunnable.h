#ifndef SCALERRUNNABLE_H
#define SCALERRUNNABLE_H

#include <QObject>
#include <QRunnable>
#include <QThread>
#include <QDebug>
#include "components/cache/cache2.h"
#include "scalerrequest.h"
#include "utils/imagelib.h"

class ScalerRunnable : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit ScalerRunnable(Cache2 *_cache);
    void setRequest(ScalerRequest r);
    void run();
signals:
    void finished(QImage*, ScalerRequest);

private:
    ScalerRequest req;
    Cache2 *cache;
};

#endif // SCALERRUNNABLE_H
