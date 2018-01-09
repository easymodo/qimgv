#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QWidget>
#include "sourcecontainers/clip.h"

class VideoPlayer : public QWidget
{
public:
    explicit VideoPlayer(QWidget *parent = nullptr);
    virtual bool openMedia(Clip *clip) = 0;
    virtual void seek(int pos) = 0;
    virtual void pauseResume() = 0;
    virtual void setPaused(bool mode) = 0;
    virtual void setMuted(bool) = 0;
};

#endif // VIDEOPLAYER_H
