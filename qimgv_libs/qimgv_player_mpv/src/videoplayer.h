#pragma once

#include <QWidget>

class VideoPlayer : public QWidget {
    Q_OBJECT
public:
    explicit VideoPlayer(QWidget *parent = nullptr);
    virtual bool openMedia(QString file) = 0;
    virtual void seek(int pos) = 0;
    virtual void seekRelative(int pos) = 0;
    virtual void pauseResume() = 0;
    virtual void frameStep() = 0;
    virtual void frameStepBack() = 0;
    virtual void stop() = 0;
    virtual void setPaused(bool mode) = 0;
    virtual void setMuted(bool) = 0;
    virtual void setVideoUnscaled(bool mode) = 0;

signals:
    void durationChanged(int value);
    void positionChanged(int value);
    void videoPaused(bool);

public slots:
    virtual void show();
    virtual void hide();
};
