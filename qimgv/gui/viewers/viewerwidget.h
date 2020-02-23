#pragma once

#include "gui/customwidgets/floatingwidgetcontainer.h"
#include <QHBoxLayout>
#include "gui/viewers/imageviewerv2.h"
#include "gui/viewers/videoplayerinitproxy.h"
#include "gui/overlays/videocontrolsproxy.h"
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

    std::shared_ptr<DirectoryViewWrapper> getPanel();

    bool showImage(std::unique_ptr<QPixmap> pixmap);
    bool showAnimation(std::unique_ptr<QMovie> movie);
    void onScalingFinished(std::unique_ptr<QPixmap> scaled);
    bool isDisplaying();
    ScalingFilter scalingFilter();
    void hidePanel();
    void hidePanelAnimated();
    PanelHPosition panelPosition();

    bool panelEnabled();
private:
    QHBoxLayout layout;
    std::unique_ptr<ImageViewerV2> imageViewer;
    std::unique_ptr<VideoPlayerInitProxy> videoPlayer;
    std::unique_ptr<ContextMenu> contextMenu;
    std::unique_ptr<MainPanel> mainPanel;
    VideoControlsProxyWrapper *videoControls;

    void enableImageViewer();
    void enableVideoPlayer();

    CurrentWidget currentWidget;
    bool mInteractionEnabled;
    QTimer cursorTimer;
    const int CURSOR_HIDE_TIMEOUT_MS = 1000;
    bool avoidPanelFlag, mPanelEnabled, mPanelFullscreenOnly, mIsFullscreen;

    void disableImageViewer();
    void disableVideoPlayer();

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
    void pauseVideo();
    void seekVideo(int pos);
    void seekVideoRelative(int pos);
    void seekVideoLeft();
    void seekVideoRight();
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

protected:
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void hideEvent(QHideEvent *event);

    void keyPressEvent(QKeyEvent *event);
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);
};
