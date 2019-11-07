#pragma once

#include <QObject>
#include <QRunnable>
#include <QThread>
#include <QDebug>
#include "components/cache/cache.h"
#include "scalerrequest.h"
#include "utils/imagelib.h"
#include "settings.h"

class ScalerRunnable : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit ScalerRunnable();
    void setRequest(ScalerRequest r);
    void run();
signals:
    void started(ScalerRequest);
    void finished(QImage*, ScalerRequest);

private:
    ScalerRequest req;
    const float CMPL_FALLBACK_THRESHOLD = 70.0; // equivalent of ~ 5000x3500 @ 32bpp
};
