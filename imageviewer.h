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
#include "imagestatic.h"
#include "mapoverlay/mapoverlay.h"
#include "settings.h"
#include "cropoverlay.h"
#include <time.h>
#include "lib/imagelib.h"
#include "thumbnailstrip/thumbnailscrollarea.h"

#define FLT_EPSILON 1.19209290E-07F

enum Position {
    TOP,
    BOTTOM
};

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
    ImageViewer(QWidget* parent);
    ~ImageViewer();
    bool isDisplaying() const;
    void addPanel(ThumbnailScrollArea*, Position);

signals:
    void sendDoubleClick();
    void sendRightDoubleClick();
    void imageChanged();
    void cropSelected(QRect);
    void resized(QSize);
    void scalingRequested(QSize);

public slots:
    void displayImage(QPixmap* _image);
    void slotFitNormal();
    void slotFitWidth();
    void slotFitAll();
    void slotZoomIn();
    void slotZoomOut();
    void resizeImage();
    void crop();

    void hideCursor();
    void updateImage(QPixmap *scaled);

protected:
    virtual void paintEvent(QPaintEvent* event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void resizeEvent(QResizeEvent* event);
    virtual void mouseDoubleClickEvent(QMouseEvent *event);

private:
    QPixmap *image;
    QTimer *resizeTimer, *cursorTimer;
    QRect drawingRect;
    QPoint mouseMoveStartPos;
    ThumbnailScrollArea *panel;
    QSize sourceSize;

    QColor bgColor;

    MapOverlay *mapOverlay;
    CropOverlay *cropOverlay;

    bool isDisplayingFlag;
    bool errorFlag;

    float currentScale;
    float maxScale; // zoom OUT
    float minScale; // zoom IN
    float scaleStep;
    QPointF fixedZoomPoint;
    Position panelPosition;

    ImageFitMode imageFitMode;
    void initOverlays();
    void setScale(float scale);
    void updateMaxScale();
    void scaleAround(QPointF p, float oldScale);
    void fitDefault();
    void fitNormal();
    void fitWidth();
    void fitAll();
    void centerImage();
    void updateMap();
    float scale() const;
    bool imageIsScaled() const;
    void stopAnimation();
    void startAnimation();
    void updateMinScale();
    void alignImage();
    void fixAlignHorizontal();
    void fixAlignVertical();
};

#endif // IMAGEVIEWER_H
