#include "mapoverlay.h"

class MapOverlayPrivate
{
public:
    MapOverlayPrivate(MapOverlay* qq);
    QPen penInner;
    QPen penOuter;
    
    QRectF outerRect;
    QRectF innerRect;
    
    MapOverlay* q;
    
    /**
     * @brief Drawing size of map
     * This means cubic size of map because width == height
     */
    
    int mapSz;
    int mapMargin;
};

MapOverlayPrivate::MapOverlayPrivate(MapOverlay* qq) : mapSz(100), q(qq)
{
}

MapOverlay::MapOverlay(QWidget *parent) : QWidget(parent),
    d(new MapOverlayPrivate(this))
{
    resize(d->mapSz, d->mapSz);
}

void MapOverlay::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setOpacity(0.7);
    
    painter.setPen(QPen(QColor(150,150,150,200)));
    painter.fillRect(d->outerRect, QBrush(QColor(80,80,80,200), Qt::SolidPattern));
    painter.drawRect(d->outerRect);
    painter.fillRect(d->innerRect, QBrush(QColor(230,230,230,150), Qt::SolidPattern));
    painter.drawRect(d->innerRect);
}

void MapOverlay::updateMap(const QSize& windowSz, const QRect& drawingRect)
{
    /* TODO set slight fadeaway for and showing thiw widget
     * 
     * some thing like this:
     * void isNeededView(QSize window, QSize drawing);
    */

    QSizeF outerSz = drawingRect.size();
    outerSz.scale(d->mapSz, d->mapSz, Qt::KeepAspectRatio);
    d->outerRect.setSize(outerSz);

    float widthDiff = (float) windowSz.width() / drawingRect.width();
    float heightDiff = (float) windowSz.height() / drawingRect.height();
    
    if (widthDiff > 1)
        widthDiff = 1;
    
    if (heightDiff > 1)
        heightDiff = 1;
    
    float width = outerSz.width() * widthDiff;
    float height = outerSz.height() * heightDiff;
    
    QSizeF innerSz(width, height);
    d->innerRect.setSize(innerSz);
    
    float xSpeedDiff = innerSz.width() / windowSz.width();
    float ySpeedDiff = innerSz.height() / windowSz.height();
    
    float x = (float) -drawingRect.left() * xSpeedDiff;
    float y = (float) -drawingRect.top() * ySpeedDiff;
    
    if (x < 0)
        x = 0;
    
    if (y < 0)
        y = 0;
    
    d->innerRect.moveTo(QPointF(x, y));

    update();
}

void MapOverlay::mouseMoveEvent(QMouseEvent* event)
{
    QWidget::mouseMoveEvent(event);
    event->accept();
}

void MapOverlay::mousePressEvent(QMouseEvent* event)
{
    QWidget::mousePressEvent(event);
    event->accept();
}

void MapOverlay::mouseReleaseEvent(QMouseEvent* event)
{
    QWidget::mouseReleaseEvent(event);
    event->accept();
}
