#include "videocontrolsproxy.h"

VideoControlsProxyWrapper::VideoControlsProxyWrapper(FloatingWidgetContainer *parent)
    : container(parent),
      videoControls(nullptr)
{
}

VideoControlsProxyWrapper::~VideoControlsProxyWrapper() {
    if(videoControls)
        videoControls->deleteLater();
}

void VideoControlsProxyWrapper::init() {
    if(videoControls)
        return;
    videoControls = new VideoControls(container);

    connect(videoControls, &VideoControls::seekBackward,  this, &VideoControlsProxyWrapper::seekBackward);
    connect(videoControls, &VideoControls::seekForward, this, &VideoControlsProxyWrapper::seekForward);
    connect(videoControls, &VideoControls::seek,      this, &VideoControlsProxyWrapper::seek);

    videoControls->setMode(stateBuf.mode);
    videoControls->setPlaybackDuration(stateBuf.duration);
    videoControls->setPlaybackPosition(stateBuf.position);
    videoControls->onPlaybackPaused(stateBuf.paused);
    videoControls->onVideoMuted(stateBuf.videoMuted);
}

void VideoControlsProxyWrapper::show() {
    init();
    videoControls->show();
}

void VideoControlsProxyWrapper::hide() {
    if(videoControls)
        videoControls->hide();
}

void VideoControlsProxyWrapper::setPlaybackDuration(int _duration) {
    if(videoControls) {
        videoControls->setPlaybackDuration(_duration);
    } else {
        stateBuf.duration = _duration;
    }
}

void VideoControlsProxyWrapper::setPlaybackPosition(int _position) {
    if(videoControls) {
        videoControls->setPlaybackPosition(_position);
    } else {
        stateBuf.position = _position;
    }
}

void VideoControlsProxyWrapper::setMode(PlaybackMode _mode) {
    if(videoControls) {
        videoControls->setMode(_mode);
    } else {
        stateBuf.mode = _mode;
    }
}

void VideoControlsProxyWrapper::onPlaybackPaused(bool _mode) {
    if(videoControls) {
        videoControls->onPlaybackPaused(_mode);
    } else {
        stateBuf.paused = _mode;
    }
}

void VideoControlsProxyWrapper::onVideoMuted(bool _mode) {
    if(videoControls) {
        videoControls->onVideoMuted(_mode);
    } else {
        stateBuf.videoMuted = _mode;
    }
}

bool VideoControlsProxyWrapper::underMouse() {
    return videoControls ? videoControls->underMouse() : false;
}

bool VideoControlsProxyWrapper::isVisible() {
    return videoControls ? videoControls->isVisible() : false;
}
