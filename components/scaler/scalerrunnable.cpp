#include "scalerrunnable.h"

ScalerRunnable::ScalerRunnable(Cache2 *_cache) : cache(_cache) {
}

void ScalerRunnable::setRequest(ScalerRequest r) {
    req = r;
}

void ScalerRunnable::run() {
    // lock the source image so it wont get deleted by main thread during scaling
    qDebug() << "scaling: "  << this->req.string;
    ImageLib imgLib;
    QImage *scaled = new QImage();
    imgLib.bilinearScale(scaled, req.image->getImage(), req.size, true);
    cache->release(req.image->info()->fileName());
    qDebug() << "scaling END: "  << this->req.string;
    emit finished(scaled, req);
}
