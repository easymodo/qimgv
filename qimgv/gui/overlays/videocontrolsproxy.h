#ifndef VIDEOCONTROLSPROXY_H
#define VIDEOCONTROLSPROXY_H

#include "gui/overlays/videocontrols.h"

struct VideoControlsStateBuffer {
    int durationSeconds = 0;
    int positionSeconds = 0;
    bool videoPaused = true;
};

class VideoControlsProxyWrapper : public QObject {
    Q_OBJECT
public:
    explicit VideoControlsProxyWrapper(OverlayContainerWidget *parent = nullptr);
    ~VideoControlsProxyWrapper();
    void init();

    void show();
    void hide();
    bool underMouse();
signals:
    void pause();
    void seek(int pos);
    void seekRight();
    void seekLeft();
    void nextFrame();
    void prevFrame();

public slots:
    void setDurationSeconds(int);
    void setPositionSeconds(int);
    void onVideoPaused(bool);

private:
    OverlayContainerWidget *container;
    VideoControls *videoControls;
    VideoControlsStateBuffer stateBuf;
};

#endif // VIDEOCONTROLSPROXY_H
