#pragma once

#include "gui/overlays/zoomindicatoroverlay.h"

struct ZoomIndicatorOverlayStateBuffer {
    qreal scale;
};

class ZoomIndicatorOverlayProxy {
public:
    explicit ZoomIndicatorOverlayProxy(FloatingWidgetContainer *parent = nullptr);
    ~ZoomIndicatorOverlayProxy();
    void init();
    void show();
    void show(int duration);
    void hide();
    void setScale(qreal scale);

private:
    FloatingWidgetContainer *container;
    ZoomIndicatorOverlay *overlay;
    ZoomIndicatorOverlayStateBuffer stateBuf;
};
