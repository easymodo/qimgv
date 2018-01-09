// performs lazy initialization

#ifndef VIDEOPLAYERMPVPROXY_H
#define VIDEOPLAYERMPVPROXY_H

#include "gui/viewers/videoplayermpv.h"
#include <QVBoxLayout>

class VideoPlayerMpvProxy : public VideoPlayer
{
public:
    VideoPlayerMpvProxy(QWidget *parent = 0);
    ~VideoPlayerMpvProxy();
    bool openMedia(Clip *clip);
    void seek(int pos);
    void pauseResume();
    void setPaused(bool mode);
    void setMuted(bool);

public slots:
    void show();
    void hide();

private:
    VideoPlayerMpv *player;
    void initPlayer();
    QVBoxLayout layout;
};

#endif // VIDEOPLAYERMPVPROXY_H
