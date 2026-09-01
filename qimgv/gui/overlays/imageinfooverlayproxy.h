#pragma once

#include "gui/overlays/imageinfooverlay.h"

struct ImageInfoOverlayStateBuffer {
    QVector<QPair<QString, QString>> info;
};

class ImageInfoOverlayProxy {
public:
    explicit ImageInfoOverlayProxy(FloatingWidgetContainer *parent = nullptr);
    ~ImageInfoOverlayProxy();
    void init();
    void show();
    void hide();

    void setExifInfo(QVector<QPair<QString, QString>> info);
    bool isHidden();
private:
    FloatingWidgetContainer *container;
    ImageInfoOverlay *overlay;
    ImageInfoOverlayStateBuffer stateBuf;
};
