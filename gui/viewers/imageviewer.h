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
    bool isDisplaying() const;
    ImageFitMode fitMode();

signals:
    void imageChanged();
    void wallpaperSelected(QRect);
    void resized(QSize);
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
    void requestScaling();
    void readSettings();
    void hideCursor();
    void showCursor();
    void updateImage(QPixmap *scaled);
    void scrollUp();
    void scrollDown();
    void startAnimation();
    void stopAnimation();
    void displayAnimation(QMovie *_animation);
    void closeImage();

protected:
    virtual void paintEvent(QPaintEvent* event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void resizeEvent(QResizeEvent* event);

private slots:
    void nextFrame();

private:
    QPixmap *image, *logo;
    QMovie *animation;
    QTransform transform;
    QTimer *cursorTimer, *animationTimer;
    QRect drawingRect;
    QPoint mouseMoveStartPos;
    QSize sourceSize;
    QColor bgColor;
    MapOverlay *mapOverlay;

    bool isDisplayingFlag, mouseWrapping, transparencyGridEnabled;
    const int transparencyGridSize = 10;

    float currentScale;
    float minScale; // zoom OUT
    float maxScale; // zoom IN
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
    float scale() const;
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
};

#endif // IMAGEVIEWER_H
