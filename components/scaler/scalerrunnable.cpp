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
    if(settings->scalingFilter() == 0 || (req.size.width() > req.image->width() && !settings->smoothUpscaling())) {
        scaled = ImageLib::scaled(req.image->getImage(), req.size, 0);
    } else {
        /*
        // This is an estimation based on image size and depth.
        // If complexity is above CMPL_FALLBACK_THRESHOLD we fall back to faster (bilinear) filter.
        // Hopefully this will prevent noticeable lag during scaling.
        float complexity = static_cast<float>(req.size.width()) *
                           static_cast<float>(req.size.height()) *
                           static_cast<float>(req.image->getImage()->depth()) / 8000000.f;
        if(complexity > CMPL_FALLBACK_THRESHOLD) {
            scaled = ImageLib::scaled(req.image->getImage(), req.size, 1);
        } else {
            scaled = ImageLib::scaled(req.image->getImage(), req.size, settings->scalingFilter());
        }
        */
        scaled = ImageLib::scaled(req.image->getImage(), req.size, settings->scalingFilter());
    }
    //qDebug() << ">> " << req.size << ": " << t.elapsed();
    emit finished(scaled, req);
}
