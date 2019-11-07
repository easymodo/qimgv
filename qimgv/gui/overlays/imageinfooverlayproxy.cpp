#include "imageinfooverlayproxy.h"

ImageInfoOverlayProxy::ImageInfoOverlayProxy(FloatingWidgetContainer *parent)
    : container(parent),
      overlay(nullptr)
{
}

ImageInfoOverlayProxy::~ImageInfoOverlayProxy() {
    if(overlay)
        overlay->deleteLater();
}

void ImageInfoOverlayProxy::show() {
    init();
    overlay->show();
}

void ImageInfoOverlayProxy::hide() {
    if(overlay)
        overlay->hide();
}

void ImageInfoOverlayProxy::init() {
    if(overlay)
        return;
    overlay = new ImageInfoOverlay(container);
    overlay->setExifInfo(stateBuf.info);
}

bool ImageInfoOverlayProxy::isHidden() {
    return overlay ? overlay->isHidden() : true;
}

void ImageInfoOverlayProxy::setExifInfo(QMap<QString, QString> _info) {
    if(overlay)
        overlay->setExifInfo(_info);
    else
        stateBuf.info = _info;
}
