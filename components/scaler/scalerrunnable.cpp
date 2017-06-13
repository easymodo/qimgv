#include "scalerrunnable.h"

ScalerRunnable::ScalerRunnable() {
}

void ScalerRunnable::setRequest(ScalerRequest *r) {
    req = r;
}

void ScalerRunnable::run() {
    if(req) {
        ImageLib imgLib;
        QImage *scaled = new QImage();
        imgLib.bilinearScale(scaled, req->image, req->size, true);
        emit finished(scaled, req);
    }
}
