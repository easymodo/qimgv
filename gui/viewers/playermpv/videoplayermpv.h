#ifndef VIDEOPLAYERMPV_H
#define VIDEOPLAYERMPV_H

#include "gui/viewers/videoplayer.h"
#include "settings.h"

class MpvWidget;

class VideoPlayerMpv : public VideoPlayer
{
    Q_OBJECT
public:
    explicit VideoPlayerMpv(QWidget *parent = 0);
    bool openMedia(Clip *clip);
    void seek(int pos);
    void pauseResume();
    void setPaused(bool mode);
    void setMuted(bool);    
    void setVideoUnscaled(bool mode);

protected:
    void paintEvent(QPaintEvent *event);

private slots:
    void readSettings();

private:
    MpvWidget *m_mpv;
};

#endif // VIDEOPLAYERMPV_H
