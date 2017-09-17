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
#include <QDebug>
#include <time.h>
#include "gui/overlays/mapoverlay.h"
#include <cmath>
#include "settings.h"

#define FLT_EPSILON 1.19209290E-07F

class ImageViewer : public QWidget
{
    Q_OBJECT

public:
    ImageViewer(QWidget* parent = 0);
    ~ImageViewer();
    ImageFitMode fitMode();

signals:
    void scalingRequested(QSize);

public slots:
    void displayImage(QPixmap* _image);
    void setFitMode(ImageFitMode mode);
    void setFitOriginal();
    void setFitWidth();
    void setFitAll();
    void zoomIn();
    void zoomOut();
    void zoomInCursor();
    void zoomOutCursor();
    void readSettings();
    void updateFrame(QPixmap *newFrame);
    void scrollUp();
    void scrollDown();
    void startAnimation();
    void stopAnimation();
    void displayAnimation(QMovie *_animation);
    void closeImage();

    void setExpandImage(bool mode);
protected:
    virtual void paintEvent(QPaintEvent* event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void resizeEvent(QResizeEvent* event);

private slots:
    void nextFrame();
    void requestScaling();
    void hideCursor();
    void showCursor();

private:
    QPixmap *image, *logo;
    QMovie *animation;
    QTransform transform;
    QTimer *cursorTimer, *animationTimer;
    QRect drawingRect;
    QPoint mouseMoveStartPos;
    QSize sourceSize;
    MapOverlay *mapOverlay;

    bool isDisplaying, mouseWrapping, checkboardGridEnabled, expandImage;

    const int CHECKBOARD_GRID_SIZE = 10;
    float maxScaleLimit = 4.0;
    float maxResolutionLimit = 75.0; // in megapixels

    float currentScale;
    float fitWindowScale;
    float minScale;
    float maxScale;
    float scaleStep;
    QPoint zoomPoint;
    QPointF zoomDrawRectPoint; // [0-1, 0-1]
    QSize desktopSize;

    ImageFitMode imageFitMode;
    void initOverlays();
    void setScale(float scale);
    void updateMinScale();
    void scaleAroundZoomPoint(float oldScale);
    void fitNormal();
    void fitWidth();
    void fitAll();
    void updateMap();
    void updateMaxScale();
    void centerImage();
    void snapEdgeHorizontal();
    void snapEdgeVertical();
    void scroll(int dx, int dy);
    void scrollX(int dx);
    void scrollY(int dy);

    void mouseDragWrapping(QMouseEvent *event);
    void mouseDrag(QMouseEvent *event);
    void mouseZoom(QMouseEvent *event);
    void drawTransparencyGrid();
    void startAnimationTimer();
    void adjustOverlays();
    void readjust(QSize _sourceSize, QRect _drawingRect);
    void reset();
    void applyFitMode();
    void setZoomPoint(QPoint pos);
    void doZoomIn();
    void doZoomOut();
    void updateFitWindowScale();
    bool sourceImageFits();
};

#endif // IMAGEVIEWER_H
