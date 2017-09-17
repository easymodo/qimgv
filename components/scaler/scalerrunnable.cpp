#include "scalerrunnable.h"

ScalerRunnable::ScalerRunnable(Cache *_cache) : cache(_cache) {
}

void ScalerRunnable::setRequest(ScalerRequest r) {
    req = r;
}

void ScalerRunnable::run() {
    emit started(req);
    ImageLib imgLib;
    QImage *scaled = new QImage();
    if(req.size.width() > req.image->width() && !settings->smoothUpscaling())
        imgLib.scale(scaled, req.image->getImage(), req.size, false);
    else
        imgLib.scale(scaled, req.image->getImage(), req.size, true);
    //qDebug() << "runnable: done - " << req.image->info()->fileName();
    emit finished(scaled, req);
}
