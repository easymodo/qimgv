#pragma once

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QElapsedTimer>
#include <QWheelEvent>
#include <QTimeLine>
#include <QScrollBar>
#include <QMovie>
#include <QColor>
#include <QTimer>
#include <QDebug>
#include <memory>
#include <cmath>
#include "settings.h"

enum MouseInteractionState {
    MOUSE_NONE,
    MOUSE_DRAG_BEGIN,
    MOUSE_DRAG,
    MOUSE_PAN,
    MOUSE_ZOOM,
    MOUSE_WHEEL_ZOOM
};

enum ViewLockMode {
    LOCK_NONE,
    LOCK_ZOOM,
    LOCK_ALL
};

class ImageViewerV2 : public QGraphicsView
{
    Q_OBJECT
public:
    ImageViewerV2(QWidget* parent = nullptr);
    ~ImageViewerV2();
    virtual ImageFitMode fitMode() const;
    virtual QRect scaledRectR() const;
    virtual float currentScale() const;
    virtual QSize sourceSize() const;
    virtual void showImage(std::unique_ptr<QPixmap> _pixmap);
    virtual void showAnimation(std::shared_ptr<QMovie> _animation);
    virtual void setScaledPixmap(std::unique_ptr<QPixmap> newFrame);
    virtual bool isDisplaying() const;

    virtual bool imageFits() const;
    bool scaledImageFits() const;
    virtual ScalingFilter scalingFilter() const;
    virtual QWidget *widget();
    bool hasAnimation() const;

    QSize scaledSizeR() const;

    void pauseResume();
    void enableDrags();
    void disableDrags();

signals:
    void scalingRequested(QSize, ScalingFilter);
    void scaleChanged(qreal);
    void sourceSizeChanged(QSize);
    void imageAreaChanged(QRect);
    void draggedOut();
    void playbackFinished();
    void animationPaused(bool);
    void frameChanged(int);
    void durationChanged(int);

public slots:
    virtual void setFitMode(ImageFitMode mode);
    virtual void setFitOriginal();
    virtual void setFitWidth();
    virtual void setFitWindow();
    virtual void zoomIn();
    virtual void zoomOut();
    virtual void zoomInCursor();
    virtual void zoomOutCursor();
    virtual void readSettings();
    virtual void scrollUp();
    virtual void scrollDown();
    virtual void scrollLeft();
    virtual void scrollRight();
    virtual void startAnimation();
    virtual void stopAnimation();
    virtual void closeImage();
    virtual void setExpandImage(bool mode);
    virtual void show();
    virtual void hide();
    virtual void setFilterNearest();
    virtual void setFilterBilinear();
    virtual void setScalingFilter(ScalingFilter filter);
    void setLoopPlayback(bool mode);
    void toggleTransparencyGrid();

    void nextFrame();
    void prevFrame();

    bool showAnimationFrame(int frame);
    void onFullscreenModeChanged(bool mode);
    void toggleLockZoom();
    bool lockZoomEnabled();
    void toggleLockView();
    bool lockViewEnabled();

protected:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void resizeEvent(QResizeEvent* event);
    void wheelEvent(QWheelEvent *event);
    void showEvent(QShowEvent *event);
    void drawBackground(QPainter *painter, const QRectF &rect);

protected slots:
    void onAnimationTimer();

private slots:
    void requestScaling();
    void scrollToX(int x);
    void scrollToY(int y);
    void centerOnPixmap();
    void onScrollTimelineFinished();

private:
    QGraphicsScene *scene;
    std::shared_ptr<QPixmap> pixmap;
    std::unique_ptr<QPixmap> pixmapScaled;
    std::shared_ptr<QMovie> movie;
    QGraphicsPixmapItem pixmapItem, pixmapItemScaled;
    QTimer *animationTimer, *scaleTimer;
    QScrollBar *hs, *vs;
    QPoint mouseMoveStartPos, mousePressPos, drawPos;
    bool transparencyGrid, expandImage,    smoothAnimatedImages,
         smoothUpscaling,  forceFastScale, keepFitMode,
         loopPlayback,     mIsFullscreen,  scrollBarWorkaround,
         useFixedZoomLevels, trackpadDetection;
    QList<float> zoomLevels;
    MouseInteractionState mouseInteraction;
    const int SCROLL_UPDATE_RATE = 7;
    const int DEFAULT_SCROLL_DISTANCE = 240;
    const qreal TRACKPAD_SCROLL_MULTIPLIER = 0.7;
    const qreal WHEEL_SCROLL_MULTIPLIER = 2.0f;
    const int ANIMATION_SPEED = 150;
    const float FAST_SCALE_THRESHOLD = 1.0f;
    const int LARGE_VIEWPORT_SIZE = 2073600;
    // how many px you can move while holding RMB until it counts as a zoom attempt
    int zoomThreshold = 4;
    int dragThreshold = 10;

    bool dragsEnabled = true;
    bool wayland = false;

    float zoomStep = 0.1, dpr;
    float minScale, maxScale, fitWindowScale, expandLimit, lockedScale;
    QPointF savedViewportPos;
    ViewLockMode mViewLock;

    QPair<QPointF, QPoint> zoomAnchor; // [pixmap coords, viewport coords]

    QElapsedTimer lastTouchpadScroll;

    ImageFitMode imageFitMode, imageFitModeDefault;
    ImageFocusPoint focusIn1to1;
    ScalingFilter mScalingFilter;

    QPixmap *checkboard;

    void zoomAnchored(float newScale);
    void fitNormal();
    void fitWidth();
    void fitWindow();

    void scroll(int dx, int dy, bool animated);

    void mousePanWrapping(QMouseEvent *event);
    void mousePan(QMouseEvent *event);
    void mouseMoveZoom(QMouseEvent *event);
    void reset();
    void applyFitMode();

    QTimeLine *scrollTimeLineX, *scrollTimeLineY;
    void stopPosAnimation();
    QPointF sceneRoundPos(QPointF scenePoint) const;
    QRectF sceneRoundRect(QRectF sceneRect) const;
    void doZoom(float newScale);
    void swapToOriginalPixmap();
    void setZoomAnchor(QPoint viewportPos);
    void updatePixmap(std::unique_ptr<QPixmap> newPixmap);
    Qt::TransformationMode selectTransformationMode();
    void centerIfNecessary();
    void snapToEdges();
    void scrollSmooth(int dx, int dy);
    void scrollPrecise(int dx, int dy);
    void updateFitWindowScale();
    void updateMinScale();
    void fitFree(float scale);
    void applySavedViewportPos();
    void saveViewportPos();
    void lockZoom();
    void doZoomIn(bool atCursor);
    void doZoomOut(bool atCursor);
};
