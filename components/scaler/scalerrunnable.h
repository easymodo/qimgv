#ifndef SCALERRUNNABLE_H
#define SCALERRUNNABLE_H

#include <QObject>
#include <QRunnable>
#include <QThread>
#include "scalerrequest.h"
#include "../../lib/imagelib.h"
#include <QDebug>

class ScalerRunnable : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit ScalerRunnable();
    void setRequest(ScalerRequest r);
    void run();
signals:
    void finished(QImage*, ScalerRequest);

private:
    ScalerRequest req;
};

#endif // SCALERRUNNABLE_H
