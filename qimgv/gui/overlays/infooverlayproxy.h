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
    explicit InfoOverlayProxyWrapper(FloatingWidgetContainer *parent = nullptr);
    ~InfoOverlayProxyWrapper();
    void init();
    void show();
    void hide();
    void setInfo(QString position, QString fileName, QString info);

private:
    FloatingWidgetContainer *container;
    InfoOverlay *infoOverlay;
    InfoOverlayStateBuffer stateBuf;
};

#endif // INFOOVERLAYPROXY_H
