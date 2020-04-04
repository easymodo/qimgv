#include "videoplayerinitproxy.h"

VideoPlayerInitProxy::VideoPlayerInitProxy(QWidget *parent)
    : VideoPlayer(parent),
      player(nullptr)
{
    setMouseTracking(true);
    setAttribute(Qt::WA_TranslucentBackground, true);
    layout.setContentsMargins(0,0,0,0);
    setLayout(&layout);
    connect(settings, &Settings::settingsChanged, this, &VideoPlayerInitProxy::onSettingsChanged);
}

VideoPlayerInitProxy::~VideoPlayerInitProxy() {
}

void VideoPlayerInitProxy::onSettingsChanged() {
    if(!player)
        return;
    player->setMuted(!settings->playVideoSounds());
    player->setVideoUnscaled(!settings->expandImage());
}

std::shared_ptr<VideoPlayer> VideoPlayerInitProxy::getPlayer() {
    return player;
}

inline bool VideoPlayerInitProxy::initPlayer() {
#ifndef USE_MPV
    return false;
#endif
    if(player)
        return true;
#ifdef __linux
    playerLib.setFileName("qimgv_player_mpv");
#else
    playerLib.setFileName("libqimgv_player_mpv.dll");
#endif
    typedef VideoPlayer* (*createPlayerWidgetFn)();
    createPlayerWidgetFn fn = (createPlayerWidgetFn) playerLib.resolve("CreatePlayerWidget");
    if(fn) {
        VideoPlayer* pl = fn();
        player.reset(pl);
    }
    if(!player) {
        qDebug() << "[VideoPlayerInitProxy] Error - could not load player library";
        qDebug() << playerLib.fileName();
        return false;
    }
    //qDebug() << "[VideoPlayerInitProxy] Library load success!";

    player->setMuted(!settings->playVideoSounds());
    player->setVideoUnscaled(!settings->expandImage());
    player->setVolume(settings->volume());

    player->setParent(this);
    layout.addWidget(player.get());
    player->hide();
    setFocusProxy(player.get());
    connect(player.get(), SIGNAL(durationChanged(int)), this, SIGNAL(durationChanged(int)));
    connect(player.get(), SIGNAL(positionChanged(int)), this, SIGNAL(positionChanged(int)));
    connect(player.get(), SIGNAL(videoPaused(bool)), this, SIGNAL(videoPaused(bool)));
    connect(player.get(), SIGNAL(playbackFinished()), this, SIGNAL(playbackFinished()));
    return true;
}

bool VideoPlayerInitProxy::openMedia(QString file) {
    if(!initPlayer())
        return false;
    return player->openMedia(file);
}

void VideoPlayerInitProxy::seek(int pos) {
    if(!initPlayer())
        return;
    player->seek(pos);
}

void VideoPlayerInitProxy::seekRelative(int pos) {
    if(!initPlayer())
        return;
    player->seekRelative(pos);
}

void VideoPlayerInitProxy::pauseResume() {
    if(!initPlayer())
        return;
    player->pauseResume();
}

void VideoPlayerInitProxy::frameStep() {
    if(!initPlayer())
        return;
    player->frameStep();
}

void VideoPlayerInitProxy::frameStepBack() {
    if(!initPlayer())
        return;
    player->frameStepBack();
}

void VideoPlayerInitProxy::stop() {
    if(!initPlayer())
        return;
    player->stop();
}

void VideoPlayerInitProxy::setPaused(bool mode) {
    if(!initPlayer())
        return;
    player->setPaused(mode);
}

void VideoPlayerInitProxy::setMuted(bool mode) {
    if(!initPlayer())
        return;
    player->setMuted(mode);
}

bool VideoPlayerInitProxy::muted() {
    if(!initPlayer())
        return true;
    return player->muted();
}

void VideoPlayerInitProxy::volumeUp() {
    if(!initPlayer())
        return;
    player->volumeUp();
    settings->setVolume(player->volume());
}

void VideoPlayerInitProxy::volumeDown() {
    if(!initPlayer())
        return;
    player->volumeDown();
    settings->setVolume(player->volume());
}

void VideoPlayerInitProxy::setVolume(int vol) {
    if(!initPlayer())
        return;
    player->setVolume(vol);
}

int VideoPlayerInitProxy::volume() {
    if(!initPlayer())
        return 0;
    return player->volume();
}

void VideoPlayerInitProxy::setVideoUnscaled(bool mode) {
    if(!initPlayer())
        return;
    player->setVideoUnscaled(mode);
}

void VideoPlayerInitProxy::setLoopPlayback(bool mode) {
    if(!initPlayer())
        return;
    player->setLoopPlayback(mode);
}

void VideoPlayerInitProxy::show() {
    if(!initPlayer())
        return;
    player->show();
    VideoPlayer::show();
}

void VideoPlayerInitProxy::hide() {
    if(!player)
        return;
    player->hide();
    VideoPlayer::hide();
}

void VideoPlayerInitProxy::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event)
}
