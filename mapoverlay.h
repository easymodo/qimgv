#ifndef MAPOVERLAY_H
#define MAPOVERLAY_H

#include <QWidget>
#include <QPainter>
#include <QPen>
#include <QDebug>
#include <QPaintEvent>
#include "settings.h"

class MapOverlayPrivate;

class MapOverlay : public QWidget
{
    Q_OBJECT
public:
    explicit MapOverlay(QWidget *parent = 0);
    void updateMap(const QSizeF& windowRect, const QRectF& drawingRect);
    void updatePosition();

protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    
private:
    MapOverlayPrivate* d;
};

#endif // MAPOVERLAY_H
