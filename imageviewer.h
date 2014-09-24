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
#include "infooverlay.h"
#include "controlsoverlay.h"
#include <time.h>
#include "sleep.cpp"

class ImageViewerPrivate;

class ImageViewer : public QWidget
{
    Q_OBJECT

public:
    ImageViewer();
    ImageViewer(QWidget* parent);
    ImageViewer(QWidget* parent, Image* image);
    ~ImageViewer();
    void setImage(Image* image);
    Image* getImage() const;
    ControlsOverlay* getControls();
    bool isDisplaying();


signals:
    void sendDoubleClick();
    void imageChanged();

public slots:
    void slotFitNormal();
    void slotFitWidth();
    void slotFitAll();
    void slotZoomIn();
    void slotZoomOut();
    void slotSetInfoString(QString);
    void slotShowInfo(bool);
    void slotShowControls(bool);

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
    void fitDefault();
    void fitHorizontal();
    void fitVertical();
    void fitOriginal();
    void fitWidth();
    void fitAll();
    void setScale(float scale);
    void centerImage();
    ImageViewerPrivate* d;
};

#endif // IMAGEVIEWER_H
