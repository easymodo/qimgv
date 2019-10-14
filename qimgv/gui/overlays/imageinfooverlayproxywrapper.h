#ifndef IMAGEINFOOVERLAYPROXYWRAPPER_H
#define IMAGEINFOOVERLAYPROXYWRAPPER_H

#include "gui/overlays/imageinfooverlay.h"

struct ImageInfoOverlayStateBuffer {
    QMap<QString, QString> info;
};

class ImageInfoOverlayProxyWrapper {
public:
    explicit ImageInfoOverlayProxyWrapper(OverlayContainerWidget *parent = nullptr);
    ~ImageInfoOverlayProxyWrapper();
    void init();
    void show();
    void hide();

    void setExifInfo(QMap<QString, QString> info);
    bool isHidden();
private:
    OverlayContainerWidget *container;
    ImageInfoOverlay *overlay;
    ImageInfoOverlayStateBuffer stateBuf;
};

#endif // IMAGEINFOOVERLAYPROXYWRAPPER_H
