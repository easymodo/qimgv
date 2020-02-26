#include "zoomindicatoroverlayproxy.h"

ZoomIndicatorOverlayProxy::ZoomIndicatorOverlayProxy(FloatingWidgetContainer *parent)
    : container(parent),
      overlay(nullptr)
{
}

ZoomIndicatorOverlayProxy::~ZoomIndicatorOverlayProxy() {
    if(overlay)
        overlay->deleteLater();
}

void ZoomIndicatorOverlayProxy::show() {
    init();
    overlay->show();
}

void ZoomIndicatorOverlayProxy::show(int duration) {
    init();
    overlay->show(duration);
}

void ZoomIndicatorOverlayProxy::hide() {
    if(overlay)
        overlay->hide();
}

void ZoomIndicatorOverlayProxy::setScale(qreal scale) {
    if(overlay) {
        overlay->setScale(scale);
    } else {
        stateBuf.scale = scale;
    }
}

void ZoomIndicatorOverlayProxy::init() {
    if(overlay)
        return;
    overlay = new ZoomIndicatorOverlay(container);
    overlay->setScale(stateBuf.scale);
}
