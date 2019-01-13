#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <QWidget>
#include <QDesktopWidget>
#include <QPaintEvent>
#include <QPainter>
#include <QImageReader>
#include <QMovie>
#include <QColor>
#include <QPalette>
#include <QTimer>
#include <QTimeLine>
#include <QElapsedTimer>
#include <QDebug>
#include <QPropertyAnimation>
#include <cmath>
#include <ctime>
#include <memory>
#include "settings.h"

#define FLT_EPSILON 1.19209290E-07F

enum MouseInteractionState {
    MOUSE_NONE,
    MOUSE_DRAG,
    MOUSE_ZOOM
};

class ImageViewer : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QPoint drawPos READ propertyDrawPos WRITE propertySetDrawPos)
public:
    ImageViewer(QWidget* parent = nullptr);
    ~ImageViewer();
    ImageFitMode fitMode();
    QRect imageRect();
    float currentScale();
    QSize sourceSize();
    void displayImage(std::unique_ptr<QPixmap> _pixmap);
    void displayAnimation(std::unique_ptr<QMovie> _animation);
    void replacePixmap(std::unique_ptr<QPixmap> newFrame);
    bool isDisplaying();

signals:
    void scalingRequested(QSize);
    void scaleChanged(float);
    void sourceSizeChanged(QSize);
    void imageAreaChanged(QRect);
    void rightClicked();
    void leftClicked();

public slots:
    void setFitMode(ImageFitMode mode);
    void setFitOriginal();
    void setFitWidth();
    void setFitWindow();
    void zoomIn();
    void zoomOut();
    void zoomInCursor();
    void zoomOutCursor();
    void readSettings();
    void scrollUp();
    void scrollDown();
    void scrollLeft();
    void scrollRight();
    void startAnimation();
    void stopAnimation();
    void closeImage();
    void setExpandImage(bool mode);
    void show();
    void hide();

protected:
    virtual void paintEvent(QPaintEvent* event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void resizeEvent(QResizeEvent* event);

private slots:
    void nextFrame();
    void requestScaling();

    void doFadeIn();
    void fadeFrame(int frame);
    void stopFadeIn();
private:
    std::unique_ptr<QPixmap> pixmap;
    std::unique_ptr<QMovie> movie;
    QTransform transform;
    QTimer *cursorTimer, *animationTimer;
    QElapsedTimer emptyViewTimer;
    QTimeLine fadeTimeLine;
    QRect drawingRect;
    QPoint mouseMoveStartPos, mousePressPos, drawPos;
    QSize mSourceSize;
    bool mIsDisplaying, mouseWrapping, checkboardGridEnabled, expandImage, smoothAnimatedImages;
    MouseInteractionState mouseInteraction;
    const int CHECKBOARD_GRID_SIZE = 10;
    const int FADE_DURATION = 140;
    const int SCROLL_DISTANCE = 250;
    const int animationSpeed = 150;
    const int ZOOM_THRESHOLD = 4; // pixels
    const int FADE_IN_THRESHOLD_MS = 160;
    float maxScaleLimit = 4.0;
    float maxResolutionLimit = 75.0; // in megapixels
    float mOpacity;

    float mCurrentScale;
    float fitWindowScale;
    float minScale;
    float maxScale;
    QPoint zoomPoint;
    QPointF zoomDrawRectPoint; // [0-1, 0-1]
    QSize desktopSize;

    ImageFitMode imageFitMode;
    void setScale(float scale);
    void updateMinScale();
    void scaleAroundZoomPoint(float oldScale);
    void fitNormal();
    void fitWidth();
    void fitWindow();
    void updateMaxScale();
    void centerImage();
    void snapEdgeHorizontal();
    void snapEdgeVertical();
    void scroll(int dx, int dy, bool animated);
    int scrolledX(int dx);
    int scrolledY(int dy);

    void mouseDragWrapping(QMouseEvent *event);
    void mouseDrag(QMouseEvent *event);
    void mouseMoveZoom(QMouseEvent *event);
    void drawTransparencyGrid();
    void startAnimationTimer();
    void readjust(QSize _sourceSize, QRect _drawingRect);
    void reset();
    void applyFitMode();
    void setZoomPoint(QPoint pos);
    void doZoomIn();
    void doZoomOut();
    void updateFitWindowScale();
    bool sourceImageFits();

    QPropertyAnimation *posAnimation;
    void propertySetDrawPos(QPoint newPos);
    QPoint propertyDrawPos();
    void stopPosAnimation();
};

#endif // IMAGEVIEWER_H
