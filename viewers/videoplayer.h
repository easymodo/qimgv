#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QtWidgets>
#include <QVBoxLayout>
#include <mpv/qthelper.hpp>
#include "../sourceContainers/clip.h"

class VideoPlayer : public QWidget {
    Q_OBJECT
public:

    VideoPlayer(QWidget *parent = 0);
    ~VideoPlayer();

private:
    mpv_handle *mpv;
    QWidget *mpvContainer;
    QVBoxLayout *layout;
    void handle_mpv_event(mpv_event *event);
    Clip *mClip;

public slots:
    void on_file_open(Clip *clip);
    void on_file_close();

private slots:
    void on_new_window();
    void on_mpv_events();

signals:
    void mpv_events();
};

#endif // VIDEOPLAYER_H
