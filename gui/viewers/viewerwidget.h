#ifndef VIEWERWIDGET_H
#define VIEWERWIDGET_H

#include <QWidget>
#include <QHBoxLayout>
#include "gui/viewers/imageviewer.h"
#include "gui/viewers/videoplayergl.h"

enum CurrentWidget {
    IMAGEVIEWER,
    VIDEOPLAYER,
    UNSET
};

class ViewerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ViewerWidget(QWidget *parent = 0);
    QRectF imageRect();
    float currentScale();
    QSize sourceSize();

private:
    QHBoxLayout layout;
    ImageViewer *imageViewer;
    VideoPlayerGL *videoPlayer;
    void initImageViewer();
    void initVideoPlayer();

    void enableImageViewer();
    void enableVideoPlayer();

    CurrentWidget currentWidget;
    QColor bgColor;

private slots:
    void readSettings();

signals:
    void scaleChanged(float);
    void sourceSizeChanged(QSize);
    void imageAreaChanged(QRectF);
    void scalingRequested(QSize);
    void zoomIn();
    void zoomOut();
    void zoomInCursor();
    void zoomOutCursor();
    void scrollUp();
    void scrollDown();

public slots:
    bool showImage(QPixmap *pixmap);
    bool showAnimation(QMovie *movie);
    bool showVideo(Clip *clip);
    void stopPlayback();
    void setFitMode(ImageFitMode mode);
    void fitWindow();
    void fitWidth();
    void fitOriginal();
    ImageFitMode fitMode();
    void onScalingFinished(QPixmap *scaled);
    void closeImage();

protected:
    virtual void paintEvent(QPaintEvent* event);
    void mouseMoveEvent(QMouseEvent *event);

};

#endif // VIEWERWIDGET_H
