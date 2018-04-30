#include "scalerrunnable.h"

ScalerRunnable::ScalerRunnable() {
}

void ScalerRunnable::setRequest(ScalerRequest r) {
    req = r;
}

void ScalerRunnable::run() {
    emit started(req);
    QImage *scaled = nullptr;
    // this is an estimation based on image size and depth
    // if complexity is above CMPL_FALLBACK_THRESHOLD we fall back to faster (bilinear) filter
    // hopefully this will prevent noticeable lag during scaling
    float complexity =  (float)req.size.width()*req.size.height()*req.image->getImage()->depth() / 8000000;
    if( req.size.width() > req.image->width() && !settings->smoothUpscaling() || settings->scalingFilter() == 0 )
        scaled = ImageLib::scaled(req.image->getImage(), req.size, 0);
    else if( complexity > CMPL_FALLBACK_THRESHOLD )
        scaled = ImageLib::scaled(req.image->getImage(), req.size, 1);
    else
        scaled = ImageLib::scaled(req.image->getImage(), req.size, settings->scalingFilter());
    emit finished(scaled, req);
}
