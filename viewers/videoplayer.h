#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QWidget>
#include <QMovie>
#include <QBoxLayout>
#include <qmediaplayer.h>
#include <qvideowidget.h>
#include <QMouseEvent>
#include <qvideosurfaceformat.h>
#include <QDebug>
#include "../settings.h"

class VideoPlayer : public QWidget
{
    Q_OBJECT
public:
    VideoPlayer(QWidget *parent = 0);
    ~VideoPlayer();

signals:
    void sendDoubleClick();
    void sendRightDoubleClick();

public slots:
    void play(QString _path);
    void replay();
    void stop();
    void readSettings();

private slots:
    void handleError();
    void handleMediaStatusChange(QMediaPlayer::MediaStatus status);

private:
    QMediaPlayer mediaPlayer;
    QString path;

protected:
    virtual void mouseMoveEvent(QMouseEvent *event);
    //virtual void mousePressEvent(QMouseEvent *event);
    //virtual void mouseReleaseEvent(QMouseEvent *event);

    virtual void mouseDoubleClickEvent(QMouseEvent *event);
};

#endif // VIDEOPLAYER_H
