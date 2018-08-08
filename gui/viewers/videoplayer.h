#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QWidget>
#include "sourcecontainers/clip.h"

class VideoPlayer : public QWidget {
    Q_OBJECT
public:
    explicit VideoPlayer(QWidget *parent = nullptr);
    virtual bool openMedia(Clip *clip) = 0;
    virtual void seek(int pos) = 0;
    virtual void seekRelative(int pos) = 0;
    virtual void pauseResume() = 0;
    virtual void frameStep() = 0;
    virtual void frameStepBack() = 0;
    virtual void stop() = 0;
    virtual void setPaused(bool mode) = 0;
    virtual void setMuted(bool) = 0;

signals:
    void durationChanged(int value);
    void positionChanged(int value);
    void videoPaused(bool);
    void rightClicked();

public slots:
    virtual void show();
    virtual void hide();
};

#endif // VIDEOPLAYER_H
