#include "videoplayermpvproxy.h"

VideoPlayerMpvProxy::VideoPlayerMpvProxy(QWidget *parent)
    : VideoPlayer(parent),
      player(NULL)
{
    layout.setContentsMargins(0,0,0,0);
    this->setLayout(&layout);
}

VideoPlayerMpvProxy::~VideoPlayerMpvProxy() {
    if(player)
        delete player;
}

inline void VideoPlayerMpvProxy::initPlayer() {
    if(!player) {
        player = new VideoPlayerMpv(this);
        layout.addWidget(player);
        player->hide();
    }
}

bool VideoPlayerMpvProxy::openMedia(Clip *clip) {
    initPlayer();
    player->openMedia(clip);
}

void VideoPlayerMpvProxy::seek(int pos) {
    initPlayer();
    player->seek(pos);
}

void VideoPlayerMpvProxy::pauseResume() {
    initPlayer();
    player->pauseResume();
}

void VideoPlayerMpvProxy::setPaused(bool mode) {
    initPlayer();
    player->setPaused(mode);
}

void VideoPlayerMpvProxy::setMuted(bool mode) {
    initPlayer();
    player->setMuted(mode);
}

void VideoPlayerMpvProxy::show() {
    initPlayer();
    player->show();
    VideoPlayer::show();
}

void VideoPlayerMpvProxy::hide() {
    if(player)
        player->hide();
    VideoPlayer::hide();
}
