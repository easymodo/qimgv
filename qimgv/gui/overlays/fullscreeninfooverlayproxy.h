#pragma once

#include "gui/overlays/fullscreeninfooverlay.h"

struct InfoOverlayStateBuffer {
    QString position;
    QString fileName;
    QString info;
};

class FullscreenInfoOverlayProxy {
public:
    explicit FullscreenInfoOverlayProxy(FloatingWidgetContainer *parent = nullptr);
    ~FullscreenInfoOverlayProxy();
    void init();
    void show();
    void hide();
    void setInfo(QString position, QString fileName, QString info);

private:
    FloatingWidgetContainer *container;
    FullscreenInfoOverlay *infoOverlay;
    InfoOverlayStateBuffer stateBuf;
};
