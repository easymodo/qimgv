#ifndef VIDEOPLAYERGL_H
#define VIDEOPLAYERGL_H

#include <QtWidgets/QWidget>
#include "sourcecontainers/clip.h"
#include "settings.h"

class MpvWidget;

class VideoPlayerGL : public QWidget
{
    Q_OBJECT
public:
    explicit VideoPlayerGL(QWidget *parent = 0);
public Q_SLOTS:
    bool openMedia(Clip *clip);
    void seek(int pos);
    void pauseResume();
    void setPaused(bool mode);
    void setMuted(bool);
    void readSettings();

private:
    MpvWidget *m_mpv;
};

#endif // VIDEOPLAYERGL_H
