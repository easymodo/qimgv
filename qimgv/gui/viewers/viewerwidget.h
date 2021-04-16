#pragma once

#include "gui/customwidgets/floatingwidgetcontainer.h"
#include <QHBoxLayout>
#include "gui/viewers/imageviewerv2.h"
#include "gui/viewers/videoplayerinitproxy.h"
#include "gui/overlays/videocontrolsproxy.h"
#include "gui/overlays/zoomindicatoroverlayproxy.h"
#include "gui/panels/mainpanel/mainpanel.h"
#include "gui/contextmenu.h"

enum CurrentWidget {
    IMAGEVIEWER,
    VIDEOPLAYER,
    UNSET
};

class ViewerWidget : public FloatingWidgetContainer
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

    std::shared_ptr<ThumbnailStripProxy> getThumbPanel();

    bool showImage(std::unique_ptr<QPixmap> pixmap);
    bool showAnimation(std::unique_ptr<QMovie> movie);
    void onScalingFinished(std::unique_ptr<QPixmap> scaled);
    bool isDisplaying();
    ScalingFilter scalingFilter();
    void hidePanel();
    void hidePanelAnimated();
    PanelHPosition panelPosition();

    bool panelEnabled();
    void setupMainPanel();

private:
    QHBoxLayout layout;
    std::unique_ptr<ImageViewerV2> imageViewer;
    std::unique_ptr<VideoPlayerInitProxy> videoPlayer;
    std::unique_ptr<ContextMenu> contextMenu;
    std::shared_ptr<MainPanel> mainPanel;
    VideoControlsProxyWrapper *videoControls;
    ZoomIndicatorOverlayProxy *zoomIndicator;

    void enableImageViewer();
    void enableVideoPlayer();

    CurrentWidget currentWidget;
    bool mInteractionEnabled, mWaylandCursorWorkaround;
    QTimer cursorTimer;
    const int CURSOR_HIDE_TIMEOUT_MS = 1000;
    bool avoidPanelFlag, mPanelEnabled, mPanelFullscreenOnly, mIsFullscreen;

    void disableImageViewer();
    void disableVideoPlayer();

    QRect videoControlsArea();
private slots:
    void onScaleChanged(qreal);
    void onVideoPlaybackFinished();
    void onAnimationPlaybackFinished();

signals:
    void scalingRequested(QSize, ScalingFilter);
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
    void toggleTransparencyGrid();
    void draggedOut();
    void setFilterNearest();
    void setFilterBilinear();
    void playbackFinished();

public slots:
    bool showVideo(QString file);
    void stopPlayback();
    void setFitMode(ImageFitMode mode);
    ImageFitMode fitMode();
    void closeImage();
    void hideCursor();
    void showCursor();
    void hideCursorTimed(bool restartTimer);

    // video control
    void pauseResumePlayback();
    void seek(int pos);
    void seekRelative(int pos);
    void seekLeft();
    void seekRight();
    void frameStep();
    void frameStepBack();
    void toggleMute();
    void volumeUp();
    void volumeDown();

    void startPlayback();
    void showContextMenu();
    void hideContextMenu();
    void showContextMenu(QPoint pos);
    void onFullscreenModeChanged(bool);
    void readSettings();
    void setLoopPlayback(bool mode);

protected:
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void hideEvent(QHideEvent *event);

    void keyPressEvent(QKeyEvent *event);
    void enterEvent(QEnterEvent *event);
    void leaveEvent(QEvent *event);
    bool focusNextPrevChild(bool mode);
};
