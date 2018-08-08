#ifndef VIEWERWIDGET_H
#define VIEWERWIDGET_H

#include "gui/customwidgets/overlaycontainerwidget.h"
#include <QHBoxLayout>
#include "gui/viewers/imageviewer.h"
#include "gui/viewers/videoplayerinitproxy.h"
#include "gui/overlays/videocontrols.h"
#include "gui/contextmenu.h"

enum CurrentWidget {
    IMAGEVIEWER,
    VIDEOPLAYER,
    UNSET
};

class ViewerWidget : public OverlayContainerWidget
{
    Q_OBJECT
public:
    explicit ViewerWidget(QWidget *parent = nullptr);
    QRect imageRect();
    float currentScale();
    QSize sourceSize();

    void enableInteraction();
    void disableInteraction();
    bool interactionEnabled();

    bool showImage(std::unique_ptr<QPixmap> pixmap);
    bool showAnimation(std::unique_ptr<QMovie> movie);
    void onScalingFinished(std::unique_ptr<QPixmap> scaled);

    bool isDisplaying();
private:
    QHBoxLayout layout;
    std::unique_ptr<ImageViewer> imageViewer;
    std::unique_ptr<VideoPlayer> videoPlayer;
    std::unique_ptr<ContextMenu> contextMenu;
    VideoControls *videoControls;

    void enableImageViewer();
    void enableVideoPlayer();

    CurrentWidget currentWidget;
    bool mInteractionEnabled;
    QTimer cursorTimer;
    const int CURSOR_HIDE_TIMEOUT_MS = 1000;

    void disableImageViewer();
    void disableVideoPlayer();

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
    void stopPlayback();
    void setFitMode(ImageFitMode mode);
    ImageFitMode fitMode();
    void closeImage();
    void hideCursor();
    void showCursor();
    void hideCursorTimed(bool restartTimer);

    // video control
    void pauseVideo();
    void seekVideo(int pos);
    void seekVideoRelative(int pos);
    void seekVideoLeft();
    void seekVideoRight();
    void frameStep();
    void frameStepBack();

    void startPlayback();
    void showContextMenu();
    void hideContextMenu();
    void showContextMenu(QPoint pos);
protected:
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void hideEvent(QHideEvent *event);
};

#endif // VIEWERWIDGET_H
