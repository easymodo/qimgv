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

class ImageViewerV2 : public QGraphicsView
{
    Q_OBJECT
public:
    ImageViewerV2(QWidget* parent = nullptr);
    ~ImageViewerV2();
    virtual ImageFitMode fitMode() const;
    virtual QRect scaledRect() const;
    virtual float currentScale() const;
    virtual QSize sourceSize() const;
    virtual void displayImage(std::unique_ptr<QPixmap> _pixmap);
    virtual void displayAnimation(std::unique_ptr<QMovie> _animation);
    virtual void setScaledPixmap(std::unique_ptr<QPixmap> newFrame);
    virtual bool isDisplaying() const;

    virtual bool imageFits() const;
    virtual ScalingFilter scalingFilter() const;
    virtual QWidget *widget();
    bool hasAnimation() const;

    QSize scaledSize() const;

    void pauseResume();
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

    virtual void toggleTransparencyGrid();

    virtual void setFilterNearest();
    virtual void setFilterBilinear();
    virtual void setScalingFilter(ScalingFilter filter);
    void setLoopPlayback(bool mode);

    void nextFrame();
    void prevFrame();

    bool showAnimationFrame(int frame);
    void onFullscreenModeChanged(bool mode);
protected:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void resizeEvent(QResizeEvent* event);
    void wheelEvent(QWheelEvent *event);
    void showEvent(QShowEvent *event);

protected slots:
    void onAnimationTimer();
private slots:
    void requestScaling();
    void scrollToX(int x);
    void scrollToY(int y);

private:
    QGraphicsScene *scene;
    std::shared_ptr<QPixmap> pixmap;
    std::unique_ptr<QPixmap> pixmapScaled;
    std::unique_ptr<QMovie> movie;
    QGraphicsPixmapItem pixmapItem, pixmapItemScaled;
    QTimer *animationTimer, *scaleTimer;
    QPoint mouseMoveStartPos, mousePressPos, drawPos;
    bool transparencyGridEnabled, expandImage, smoothAnimatedImages, smoothUpscaling, forceFastScale, keepFitMode, loopPlayback, mIsFullscreen;
    MouseInteractionState mouseInteraction;
    const int CHECKBOARD_GRID_SIZE = 10;
    const int SCROLL_UPDATE_RATE = 7;
    const int SCROLL_DISTANCE = 250;
    const qreal SCROLL_SPEED_MILTIPLIER = 1.3;
    const qreal TRACKPAD_SCROLL_MULTIPLIER = 0.7;
    const int ANIMATION_SPEED = 120;
    const float FAST_SCALE_THRESHOLD = 1.0f;
    const int LARGE_VIEWPORT_SIZE = 2073600;
    // how many px you can move while holding RMB until it counts as a zoom attempt
    int zoomThreshold = 4;
    int dragThreshold = 10;
    qreal zoomStep = 0.1, dpr;
    float minScale, maxScale, fitWindowScale, expandLimit;
    QPair<QPointF, QPoint> zoomAnchor; // [pixmap coords, viewport coords]

    QElapsedTimer lastTouchpadScroll;

    ImageFitMode imageFitMode, imageFitModeDefault;
    ImageFocusPoint focusIn1to1;
    ScalingFilter mScalingFilter;

    void zoomAnchored(float newScale);
    void fitNormal();
    void fitWidth();
    void fitWindow();
    void centerOnPixmap();
    void scroll(int dx, int dy, bool animated);

    void mousePanWrapping(QMouseEvent *event);
    void mousePan(QMouseEvent *event);
    void mouseMoveZoom(QMouseEvent *event);
    void drawTransparencyGrid();
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
    bool scaledImageFits() const;
    Qt::TransformationMode selectTransformationMode();
    void centerIfNecessary();
    void snapToEdges();
    void scrollSmooth(int dx, int dy);
    void scrollPrecise(int dx, int dy);
    void updateFitWindowScale();
    void updateMinScale();
};
