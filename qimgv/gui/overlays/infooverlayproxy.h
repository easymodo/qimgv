#ifndef INFOOVERLAYPROXY_H
#define INFOOVERLAYPROXY_H

#include "gui/overlays/infooverlay.h"

struct InfoOverlayStateBuffer {
    QString position;
    QString fileName;
    QString info;
};

class InfoOverlayProxyWrapper {
public:
    explicit InfoOverlayProxyWrapper(OverlayContainerWidget *parent = nullptr);
    ~InfoOverlayProxyWrapper();
    void init();
    void show();
    void hide();
    void setInfo(QString position, QString fileName, QString info);

private:
    OverlayContainerWidget *container;
    InfoOverlay *infoOverlay;
    InfoOverlayStateBuffer stateBuf;
};

#endif // INFOOVERLAYPROXY_H
