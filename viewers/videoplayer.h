#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsVideoItem>
#include <QGraphicsWidget>
#include <QGraphicsLinearLayout>
#include <QWidget>
#include <QMovie>
#include <QBoxLayout>
#include <qmediaplayer.h>
#include <qvideowidget.h>
#include <QMouseEvent>
#include <qvideosurfaceformat.h>
#include <QDebug>
#include "../settings.h"
#include "../thumbnailstrip/customscene.h"

class VideoPlayer : public QGraphicsView
{
    Q_OBJECT
public:
    VideoPlayer(QWidget *parent = 0);
    ~VideoPlayer();

signals:
    void sendDoubleClick();
    void sendRightDoubleClick();
    void parentResized(QSize);

public slots:
    void play(QString _path);
    void replay();
    void stop();
    void readSettings();

private slots:
    void handleError();
    void handleMediaStatusChange(QMediaPlayer::MediaStatus status);
    void handlePlayerStateChange(QMediaPlayer::State status);

    void adjustVideoSize();
private:
    QMediaPlayer mediaPlayer;
    QString path;
    CustomScene *scene;
    QGraphicsLinearLayout *layout;
    QGraphicsWidget *videoWidget;
    QGraphicsVideoItem *videoItem;

protected:
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    //virtual void mouseReleaseEvent(QMouseEvent *event);

    virtual void mouseDoubleClickEvent(QMouseEvent *event);
};

#endif // VIDEOPLAYER_H
