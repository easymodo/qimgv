#pragma once

#include "gui/overlays/videocontrols.h"

struct VideoControlsStateBuffer {
    int duration = 0;
    int position = 0;
    bool paused = true;
    bool videoMuted = true;
    PlaybackMode mode;
};

class VideoControlsProxyWrapper : public QObject {
    Q_OBJECT
public:
    explicit VideoControlsProxyWrapper(FloatingWidgetContainer *parent = nullptr);
    ~VideoControlsProxyWrapper();
    void init();

    void show();
    void hide();
    bool underMouse();
    bool isVisible();

signals:
    void seek(int pos);
    void seekForward();
    void seekBackward();

public slots:
    void setPlaybackDuration(int);
    void setPlaybackPosition(int);
    void setMode(PlaybackMode);
    void onPlaybackPaused(bool);
    void onVideoMuted(bool);

private:
    FloatingWidgetContainer *container;
    VideoControls *videoControls;
    VideoControlsStateBuffer stateBuf;
};
