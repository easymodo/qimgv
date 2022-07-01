// performs lazy initialization

#pragma once

#include <memory>
#include <QVBoxLayout>
#include "videoplayer.h"
#include "settings.h"
#include <QPainter>
#include <QLibrary>
#include <QLabel>
#include <QFileInfo>
#include <QDebug>

class VideoPlayerInitProxy : public VideoPlayer {
public:
    VideoPlayerInitProxy(QWidget *parent = nullptr);
    ~VideoPlayerInitProxy();
    bool showVideo(QString file);
    void seek(int pos);
    void seekRelative(int pos);
    void pauseResume();
    void frameStep();
    void frameStepBack();
    void stop();
    void setPaused(bool mode);
    void setMuted(bool);
    bool muted();
    void volumeUp();
    void volumeDown();
    void setVolume(int);
    int volume();
    void setVideoUnscaled(bool mode);
    void setLoopPlayback(bool mode);
    std::shared_ptr<VideoPlayer> getPlayer();
    bool isInitialized();

public slots:
    void show();
    void hide();

protected:
    void paintEvent(QPaintEvent *event);

private:
    QLibrary playerLib;
    std::shared_ptr<VideoPlayer> player;
    bool initPlayer();
    QVBoxLayout layout;
    QLabel *errorLabel = nullptr;

    QString libFile;
    QStringList libDirs;

private slots:
    void onSettingsChanged();

signals:
    void playbackFinished();

};
