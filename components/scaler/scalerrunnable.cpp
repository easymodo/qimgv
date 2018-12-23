#include "scalerrunnable.h"
#include "3rdparty/QtOpenCV/cvmatandqimage.h"

ScalerRunnable::ScalerRunnable() {
}

void ScalerRunnable::setRequest(ScalerRequest r) {
    req = r;
}

void ScalerRunnable::run() {
    emit started(req);
    std::shared_ptr<StaticImageContainer> scaled(
        ImageLib::scaledCv(req.image->getImage(), req.size, settings->scalingFilter(), settings->imageSharpening())
    );
    emit finished(scaled, req);
}
