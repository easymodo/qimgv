#include "videoplayerdummy.h"

VideoPlayerDummy::VideoPlayerDummy(QWidget *parent) : VideoPlayer(parent)
{
    qDebug() << "using dummy player";
}

bool VideoPlayerDummy::openMedia(Clip *clip)
{
}

void VideoPlayerDummy::seek(int pos)
{
}

void VideoPlayerDummy::pauseResume()
{
}

void VideoPlayerDummy::setPaused(bool mode)
{
}

void VideoPlayerDummy::setMuted(bool)
{
}
