#pragma once

#include "gui/overlays/imageinfooverlay.h"

struct ImageInfoOverlayStateBuffer {
    QMap<QString, QString> info;
};

class ImageInfoOverlayProxy {
public:
    explicit ImageInfoOverlayProxy(FloatingWidgetContainer *parent = nullptr);
    ~ImageInfoOverlayProxy();
    void init();
    void show();
    void hide();

    void setExifInfo(QMap<QString, QString> info);
    bool isHidden();
private:
    FloatingWidgetContainer *container;
    ImageInfoOverlay *overlay;
    ImageInfoOverlayStateBuffer stateBuf;
};
