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

    connect(videoControls, &VideoControls::pause,     this, &VideoControlsProxyWrapper::pause);
    connect(videoControls, &VideoControls::seekLeft,  this, &VideoControlsProxyWrapper::seekLeft);
    connect(videoControls, &VideoControls::seekRight, this, &VideoControlsProxyWrapper::seekRight);
    connect(videoControls, &VideoControls::seek,      this, &VideoControlsProxyWrapper::seek);
    connect(videoControls, &VideoControls::nextFrame, this, &VideoControlsProxyWrapper::nextFrame);
    connect(videoControls, &VideoControls::prevFrame, this, &VideoControlsProxyWrapper::prevFrame);

    videoControls->setDurationSeconds(stateBuf.durationSeconds);
    videoControls->setPositionSeconds(stateBuf.positionSeconds);
    videoControls->onVideoPaused(stateBuf.videoPaused);
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

void VideoControlsProxyWrapper::setDurationSeconds(int _duration) {
    if(videoControls) {
        videoControls->setDurationSeconds(_duration);
    } else {
        stateBuf.durationSeconds = _duration;
    }
}

void VideoControlsProxyWrapper::setPositionSeconds(int _position) {
    if(videoControls) {
        videoControls->setPositionSeconds(_position);
    } else {
        stateBuf.positionSeconds = _position;
    }
}

void VideoControlsProxyWrapper::onVideoPaused(bool _mode) {
    if(videoControls) {
        videoControls->onVideoPaused(_mode);
    } else {
        stateBuf.videoPaused = _mode;
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
