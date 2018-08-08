#include "videoplayerinitproxy.h"

VideoPlayerInitProxy::VideoPlayerInitProxy(QWidget *parent)
    : VideoPlayer(parent)
{
    this->setMouseTracking(true);
    setAttribute(Qt::WA_TranslucentBackground, true);
    layout.setContentsMargins(0,0,0,0);
    this->setLayout(&layout);
}

VideoPlayerInitProxy::~VideoPlayerInitProxy() {
}

inline void VideoPlayerInitProxy::initPlayer() {
    if(!player) {
        player.reset(new VideoPlayerImpl(this));
        layout.addWidget(player.get());
        player->hide();
        connect(player.get(), SIGNAL(durationChanged(int)), this, SIGNAL(durationChanged(int)));
        connect(player.get(), SIGNAL(positionChanged(int)), this, SIGNAL(positionChanged(int)));
        connect(player.get(), SIGNAL(videoPaused(bool)), this, SIGNAL(videoPaused(bool)));
        connect(player.get(), SIGNAL(rightClicked()), this, SIGNAL(rightClicked()));
    }
}

bool VideoPlayerInitProxy::openMedia(Clip *clip) {
    initPlayer();
    return player->openMedia(clip);
}

void VideoPlayerInitProxy::seek(int pos) {
    initPlayer();
    player->seek(pos);
}

void VideoPlayerInitProxy::seekRelative(int pos) {
    initPlayer();
    player->seekRelative(pos);
}

void VideoPlayerInitProxy::pauseResume() {
    initPlayer();
    player->pauseResume();
}

void VideoPlayerInitProxy::frameStep() {
    initPlayer();
    player->frameStep();
}

void VideoPlayerInitProxy::frameStepBack() {
    initPlayer();
    player->frameStepBack();
}

void VideoPlayerInitProxy::stop() {
    initPlayer();
    player->stop();
}

void VideoPlayerInitProxy::setPaused(bool mode) {
    initPlayer();
    player->setPaused(mode);
}

void VideoPlayerInitProxy::setMuted(bool mode) {
    initPlayer();
    player->setMuted(mode);
}

void VideoPlayerInitProxy::show() {
    initPlayer();
    player->show();
    VideoPlayer::show();
}

void VideoPlayerInitProxy::hide() {
    if(player)
        player->hide();
    VideoPlayer::hide();
}

void VideoPlayerInitProxy::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event)
}
