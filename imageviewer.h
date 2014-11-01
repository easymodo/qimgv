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
    bool isDisplaying();

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
    Image* currentImage;
    QImage image;
    QTimer animationTimer;
    QRectF drawingRect;
    QPoint mouseMoveStartPos;

    MapOverlay *mapOverlay;

    bool isDisplayingFlag;
    bool errorFlag;

    float currentScale;
    float maxScale = 0.50;
    static const float defaultMaxScale = 0.50;
    static const float minScale = 5.0;
    static const float scaleStep = 0.1;
    QPointF fixedZoomPoint;

    ImageFitMode imageFitMode;
    void initMap();
    void setScale(float scale);
    void calculateMaxScale();
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
    bool scaled() const;
    void stopAnimation();
    void startAnimation();
};

#endif // IMAGEVIEWER_H
