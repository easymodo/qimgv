#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <QWidget>
#include <QPaintEvent>
#include <QPainter>
#include <QImageReader>
#include <QColor>
#include <QPalette>
#include <QTimer>
#include <QThread>
#include <QtConcurrent>
#include <QFuture>
#include <QDebug>
#include <QErrorMessage>
#include <vector>
#include "image.h"
#include "mapoverlay.h"
#include "settings.h"
#include <time.h>
#include <cmath>

#define FLT_EPSILON 1.19209290E-07F

enum ImageFitMode
{
    NORMAL,
    WIDTH,
    ALL,
    FREE
};

class ImageViewer : public QWidget
{
    Q_OBJECT

public:
    ImageViewer();
    ImageViewer(QWidget* parent);
    ~ImageViewer();
    Image* getCurrentImage() const;
    bool isDisplaying() const;

signals:
    void sendDoubleClick();
    void sendRightDoubleClick();
    void imageChanged();

public slots:
    void displayImage(Image* image);
    void freeImage();
    void slotFitNormal();
    void slotFitWidth();
    void slotFitAll();
    void slotZoomIn();
    void slotZoomOut();
    void resizeImage();

private slots:
    void onAnimation();

protected:
    virtual void paintEvent(QPaintEvent* event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void resizeEvent(QResizeEvent* event);
    virtual void mouseDoubleClickEvent(QMouseEvent *event);

private:
    Image* source;
    QImage *image;
    QTimer *resizeTimer;
    QRect drawingRect;
    QPoint mouseMoveStartPos;

    QColor bgColor;

    MapOverlay *mapOverlay;

    bool isDisplayingFlag;
    bool errorFlag;

    float currentScale;
    float maxScale; // zoom OUT
    float minScale; // zoom IN
    float scaleStep;
    QPointF fixedZoomPoint;

    ImageFitMode imageFitMode;
    void initMap();
    void setScale(float scale);
    void updateMaxScale();
    void scaleAround(QPointF p, float oldScale);
    void fitDefault();
    void fitNormal();
    void fitWidth();
    void fitAll();
    void imageAlign();
    void centerImage();
    void updateMap();
    void fixAlignHorizontal();
    void fixAlignVertical();
    float scale() const;
    bool imageIsScaled() const;
    void stopAnimation();
    void startAnimation();
    void updateMinScale();
    void fastScale(bool);
    void qualityScale();
};

#endif // IMAGEVIEWER_H
