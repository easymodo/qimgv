#include "scalerrunnable.h"

#include <QElapsedTimer>

ScalerRunnable::ScalerRunnable() {
}

void ScalerRunnable::setRequest(ScalerRequest r) {
    req = r;
}

void ScalerRunnable::run() {
    emit started(req);
    //QElapsedTimer t;
    //t.start();
    QImage *scaled = nullptr;
    if(req.filter == 0 || (req.size.width() > req.image->width() && !settings->smoothUpscaling())) {
        scaled = ImageLib::scaled(req.image->getImage(), req.size, QI_FILTER_NEAREST);
    } else {
        scaled = ImageLib::scaled(req.image->getImage(), req.size, req.filter);
    }
    //qDebug() << ">> " << req.size << ": " << t.elapsed();
    emit finished(scaled, req);
}
