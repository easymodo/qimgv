#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <QWidget>
#include <QPaintEvent>
#include <QPainter>
#include <QImageReader>
#include <QColor>
#include <QPalette>
#include <QTimer>
#include <QDebug>
#include <QErrorMessage>
#include <vector>
#include "image.h"

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
    void fitImageHorizontal();
    void fitImageVertical();
    void fitImageDefault();
    void fitImageOriginal();
    void increaseScale(double value);
    void setScale(double scale);

private slots:
    void onAnimation();

protected:
    virtual void paintEvent(QPaintEvent* event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void resizeEvent(QResizeEvent* event);
    virtual void wheelEvent(QWheelEvent* event);

private:
    ImageViewerPrivate* d;
};

#endif // IMAGEVIEWER_H
