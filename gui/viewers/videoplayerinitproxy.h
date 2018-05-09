// performs lazy initialization

#ifndef VIDEOPLAYERMPVPROXY_H
#define VIDEOPLAYERMPVPROXY_H

#include <memory>
#include <QVBoxLayout>
#include "gui/viewers/videoplayer.h"

#ifdef USE_MPV
#include "gui/viewers/videoplayermpv.h"
typedef VideoPlayerMpv VideoPlayerImpl;
#else
#include "gui/viewers/videoplayerdummy.h"
typedef VideoPlayerDummy VideoPlayerImpl;
#endif

class VideoPlayerInitProxy : public VideoPlayer
{
public:
    VideoPlayerInitProxy(QWidget *parent = 0);
    ~VideoPlayerInitProxy();
    bool openMedia(Clip *clip);
    void seek(int pos);
    void pauseResume();
    void setPaused(bool mode);
    void setMuted(bool);

public slots:
    void show();
    void hide();

protected:
    void paintEvent(QPaintEvent *event);

private:
    std::unique_ptr<VideoPlayerImpl> player;
    void initPlayer();
    QVBoxLayout layout;
};

#endif // VIDEOPLAYERMPVPROXY_H
