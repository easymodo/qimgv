// performs lazy initialization

#ifndef VIDEOPLAYERMPVPROXY_H
#define VIDEOPLAYERMPVPROXY_H

#include <memory>
#include <QVBoxLayout>
#include "videoplayer.h"
#include <QLibrary>
#include <QDebug>

class VideoPlayerInitProxy : public VideoPlayer {
public:
    VideoPlayerInitProxy(QWidget *parent = nullptr);
    ~VideoPlayerInitProxy();
    bool openMedia(QString file);
    void seek(int pos);
    void seekRelative(int pos);
    void pauseResume();
    void frameStep();
    void frameStepBack();
    void stop();
    void setPaused(bool mode);
    void setMuted(bool);

public slots:
    void show();
    void hide();

protected:
    void paintEvent(QPaintEvent *event);

private:
    QLibrary playerLib;
    std::unique_ptr<VideoPlayer> player;
    bool initPlayer();
    QVBoxLayout layout;
};

#endif // VIDEOPLAYERMPVPROXY_H
