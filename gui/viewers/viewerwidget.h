#ifndef VIEWERWIDGET_H
#define VIEWERWIDGET_H

#include <QWidget>
#include <QHBoxLayout>
#include "gui/viewers/imageviewer.h"
#include "gui/viewers/videoplayergl.h"

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
    QColor bgColor;

private slots:
    void readSettings();

signals:

public slots:
    bool showImage(QPixmap *pixmap);
    bool showAnimation(QMovie *movie);
    bool showVideo(Clip *clip);
    void stopPlayback();
    void setFitMode(ImageFitMode mode);
    ImageFitMode fitMode();

protected:
    virtual void paintEvent(QPaintEvent* event);
    void mouseMoveEvent(QMouseEvent *event);

};

#endif // VIEWERWIDGET_H
