#include "imageinfooverlayproxywrapper.h"

ImageInfoOverlayProxyWrapper::ImageInfoOverlayProxyWrapper(OverlayContainerWidget *parent)
    : container(parent),
      overlay(nullptr)
{
}

ImageInfoOverlayProxyWrapper::~ImageInfoOverlayProxyWrapper() {
    if(overlay)
        overlay->deleteLater();
}

void ImageInfoOverlayProxyWrapper::show() {
    init();
    overlay->show();
}

void ImageInfoOverlayProxyWrapper::hide() {
    if(overlay)
        overlay->hide();
}

void ImageInfoOverlayProxyWrapper::init() {
    if(overlay)
        return;
    overlay = new ImageInfoOverlay(container);
    overlay->setExifInfo(stateBuf.info);
}

bool ImageInfoOverlayProxyWrapper::isHidden() {
    return overlay ? overlay->isHidden() : true;
}

void ImageInfoOverlayProxyWrapper::setExifInfo(QMap<QString, QString> _info) {
    if(overlay)
        overlay->setExifInfo(_info);
    else
        stateBuf.info = _info;
}
