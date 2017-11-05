#include "scalerrunnable.h"

ScalerRunnable::ScalerRunnable(Cache *_cache) : cache(_cache) {
}

void ScalerRunnable::setRequest(ScalerRequest r) {
    req = r;
}

void ScalerRunnable::run() {
    emit started(req);
    ImageLib imgLib;
    QImage *scaled = NULL;
    if( req.size.width() > req.image->width() && !settings->smoothUpscaling() || settings->scalingFilter() == 0 )
        scaled = imgLib.scale(req.image->getImage(), req.size, 0);
    else if( (float)req.size.width()*req.size.height() / 1000000 > 23.0 ) // todo: test on different pc & tune value
        scaled = imgLib.scale(req.image->getImage(), req.size, 1);
    else
        scaled = imgLib.scale(req.image->getImage(), req.size, settings->scalingFilter());
    emit finished(scaled, req);
}
