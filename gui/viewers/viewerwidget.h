#ifndef VIEWERWIDGET_H
#define VIEWERWIDGET_H

#include "gui/customwidgets/containerwidget.h"
#include <QHBoxLayout>
#include "gui/viewers/imageviewer.h"
#include "gui/viewers/videoplayerinitproxy.h"
#include "gui/viewers/folderview.h"

enum CurrentWidget {
    IMAGEVIEWER,
    VIDEOPLAYER,
    FOLDERVIEW,
    UNSET
};

class ViewerWidget : public ContainerWidget
{
    Q_OBJECT
public:
    explicit ViewerWidget(QWidget *parent = 0);
    QRect imageRect();
    float currentScale();
    QSize sourceSize();

    void enableZoomInteraction();
    void disableZoomInteraction();
    bool zoomInteractionEnabled();

    bool showImage(std::unique_ptr<QPixmap> pixmap);
    bool showAnimation(std::unique_ptr<QMovie> movie);
    void onScalingFinished(std::unique_ptr<QPixmap> scaled);

private:
    QHBoxLayout layout;
    std::unique_ptr<ImageViewer> imageViewer;
    std::unique_ptr<VideoPlayer> videoPlayer;

    void enableImageViewer();
    void enableVideoPlayer();

    CurrentWidget currentWidget;
    QColor bgColor;
    float bgOpacity;
    bool zoomInteraction;
    QTimer cursorTimer;
    const int CURSOR_HIDE_TIMEOUT_MS = 1000;

private slots:
    void readSettings();

signals:
    void scalingRequested(QSize);
    void zoomIn();
    void zoomOut();
    void zoomInCursor();
    void zoomOutCursor();
    void scrollUp();
    void scrollDown();
    void scrollLeft();
    void scrollRight();
    void fitWindow();
    void fitWidth();
    void fitOriginal();

public slots:

    bool showVideo(Clip *clip);
    bool showFolderView();
    void stopPlayback();
    void setFitMode(ImageFitMode mode);
    ImageFitMode fitMode();
    void closeImage();

    void hideCursor();
    void showCursor();
    void hideCursorTimed(bool restartTimer);

protected:
    virtual void paintEvent(QPaintEvent* event);
    void mouseMoveEvent(QMouseEvent *event);

    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
};

#endif // VIEWERWIDGET_H
