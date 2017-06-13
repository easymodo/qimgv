#include "scalerrunnable.h"

ScalerRunnable::ScalerRunnable() {
}

void ScalerRunnable::setRequest(ScalerRequest r) {
    req = r;
}

void ScalerRunnable::run() {
    // lock the source image so it wont get deleted by main thread during scaling
    req.image->lock();
    ImageLib imgLib;
    QImage *scaled = new QImage();
    imgLib.bilinearScale(scaled, req.image->getImage(), req.size, true);
    req.image->unlock();
    emit finished(scaled, req);
}
