#include "videoplayerdummy.h"

VideoPlayerDummy::VideoPlayerDummy(QWidget *parent) : VideoPlayer(parent)
{
}

bool VideoPlayerDummy::openMedia(Clip *clip)
{
    return false;
}

void VideoPlayerDummy::seek(int pos)
{
}

void VideoPlayerDummy::seekRelative(int pos)
{
}

void VideoPlayerDummy::pauseResume()
{
}

void VideoPlayerDummy::frameStep()
{
}

void VideoPlayerDummy::frameStepBack()
{
}

void VideoPlayerDummy::stop()
{
}

void VideoPlayerDummy::setPaused(bool mode)
{
}

void VideoPlayerDummy::setMuted(bool)
{
}
