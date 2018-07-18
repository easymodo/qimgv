#ifndef VIEWERWIDGET_H
#define VIEWERWIDGET_H

#include "gui/customwidgets/overlaycontainerwidget.h"
#include <QHBoxLayout>
#include "gui/viewers/imageviewer.h"
#include "gui/viewers/videoplayerinitproxy.h"
#include "gui/viewers/folderview.h"
#include "gui/overlays/videocontrols.h"

enum CurrentWidget {
    IMAGEVIEWER,
    VIDEOPLAYER,
    FOLDERVIEW,
    UNSET
};

class ViewerWidget : public OverlayContainerWidget
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
    std::unique_ptr<FolderView> folderView;
    VideoControls *videoControls;

    void enableImageViewer();
    void enableVideoPlayer();

    CurrentWidget currentWidget;
    QColor bgColor;
    float bgOpacity;
    bool zoomInteraction;
    QTimer cursorTimer;
    const int CURSOR_HIDE_TIMEOUT_MS = 1000;

    void disableImageViewer();
    void disableVideoPlayer();
    void disableFolderView();
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
    void thumbnailRequested(QList<int>, int);
    void thumbnailPressed(int);

public slots:
    bool showVideo(Clip *clip);
    void enableFolderView();
    void stopPlayback();
    void setFitMode(ImageFitMode mode);
    ImageFitMode fitMode();
    void closeImage();
    void hideCursor();
    void showCursor();
    void hideCursorTimed(bool restartTimer);

    void setThumbnail(int pos, std::shared_ptr<Thumbnail> thumb);

    void populateFolderView(int count);

    // video control
    void pauseVideo();
    void seekVideo(int pos);
    void seekVideoRelative(int pos);
    void seekVideoLeft();
    void seekVideoRight();
    void frameStep();
    void frameStepBack();

protected:
    virtual void paintEvent(QPaintEvent* event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
};

#endif // VIEWERWIDGET_H
