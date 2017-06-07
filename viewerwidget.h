#ifndef VIEWERWIDGET_H
#define VIEWERWIDGET_H

#include <QWidget>
#include <QHBoxLayout>
#include "viewers/imageviewer.h"
#include "viewers/videoplayergl.h"

class ViewerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ViewerWidget(ImageViewer *imageViewer, VideoPlayerGL *videoPlayer, QWidget *parent = 0);

private:
    QHBoxLayout layout;
    ImageViewer *imageViewer;
    VideoPlayerGL *videoPlayer;
    void setImageViewer(ImageViewer *imageViewer);
    void setVideoPlayer(VideoPlayerGL *videoPlayer);

    void enableImageViewer();
    void enableVideoPlayer();

    int currentWidget; // 1 - imageViewer; 2 - videoplayer; others - none

signals:

public slots:
    bool showImage(QPixmap *pixmap);
    bool showAnimation(QMovie *movie);
    bool showVideo(Clip *clip);
    void stopPlayback();
};

#endif // VIEWERWIDGET_H
