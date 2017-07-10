#include "scalerrunnable.h"

ScalerRunnable::ScalerRunnable(Cache *_cache) : cache(_cache) {
}

void ScalerRunnable::setRequest(ScalerRequest r) {
    req = r;
}

void ScalerRunnable::run() {
    ImageLib imgLib;
    QImage *scaled = new QImage();
    imgLib.bilinearScale(scaled, req.image->getImage(), req.size, true);
    cache->release(req.image->info()->fileName());
    emit finished(scaled, req);
}
