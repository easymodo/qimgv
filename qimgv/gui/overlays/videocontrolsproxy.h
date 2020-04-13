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
signals:
    void pause();
    void seek(int pos);
    void seekRight();
    void seekLeft();
    void nextFrame();
    void prevFrame();

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
