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

MapOverlayPrivate::MapOverlayPrivate(MapOverlay* qq) :
    mapSz(100),
    mapMargin(10),
    q(qq)
{
}

MapOverlay::MapOverlay(QWidget *parent) : QWidget(parent),
    d(new MapOverlayPrivate(this))
{
    updatePosition();
}

void MapOverlay::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setPen(QPen(QColor(20,20,20,150)));
    painter.fillRect(d->outerRect,
                     QBrush(QColor(80,80,80,150),
                            Qt::SolidPattern));
    painter.drawRect(d->outerRect);
    painter.fillRect(d->innerRect,
                     QBrush(QColor(190,190,190,150),
                            Qt::SolidPattern));
    painter.drawRect(d->innerRect);
}

void MapOverlay::updatePosition() {
    this->setGeometry(parentWidget()->width()-d->mapSz-d->mapMargin,
                      parentWidget()->height()-d->mapSz-d->mapMargin,
                      d->mapSz+1,
                      d->mapSz+1);
}

// calculates outer(image) and inner(view area) squares
void MapOverlay::updateMap(const QSizeF& windowSz, const QRectF& drawingRect)
{
    if(windowSz.height() < drawingRect.height()
            || windowSz.width() < drawingRect.width()) {
        this->show();
    
        d->outerRect.setX(0);
        d->outerRect.setY(0);
    
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
        
        float xSpeedDiff = (float) innerSz.width() / windowSz.width();
        float ySpeedDiff = (float) innerSz.height() / windowSz.height();

        float x = (float) -drawingRect.left() * xSpeedDiff;
        float y = (float) -drawingRect.top() * ySpeedDiff;

        if (x < 0)
            x = 0;

        if (y < 0)
            y = 0;

        d->innerRect.moveTo(QPointF(x, y));
        
        //move to bottom left border
        d->outerRect.translate(d->mapSz - d->outerRect.width(),
                            d->mapSz - d->outerRect.height());
        d->innerRect.translate(d->mapSz - d->outerRect.width(),
                            d->mapSz - d->outerRect.height());
        update();
    }
    else {
        this->hide();
    }
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
