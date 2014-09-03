#include "mapoverlay.h"

class MapOverlayPrivate
{
public:
    MapOverlayPrivate(MapOverlay* qq);
    QPen penInner;
    QPen penOuter;
    
    QRect outerRect;
    QRect innerRect;
    
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
   // qDebug() << d->outerRect << d->innerRect;
    painter.fillRect(0,0,this->width(),this->height(), QBrush(QColor(250,0,0,120), Qt::SolidPattern));
    painter.drawRect(d->outerRect);

    painter.setPen(QPen(QColor(150,150,150,200)));
    painter.fillRect(d->outerRect, QBrush(QColor(80,80,80,200), Qt::SolidPattern));
    painter.drawRect(d->outerRect);
    painter.fillRect(d->innerRect, QBrush(QColor(230,230,230,150), Qt::SolidPattern));
    painter.drawRect(d->innerRect);
}

void MapOverlay::updatePosition() {
    this->setGeometry(parentWidget()->width()-d->mapSz-d->mapMargin,
                      parentWidget()->height()-d->mapSz-d->mapMargin,
                      d->mapSz,
                      d->mapSz);
}

/* calculates outer(image) and inner(view area) squares
 * trust me, it just does
 * 
 * okay but I make it better. Your code make me cry.
 *
 * your code makes *everyone* cry
 */

void MapOverlay::updateMap(const QSize& windowSz, const QRect& drawingRect)
{
   // qDebug() << windowSz << drawingRect;
    if(windowSz.height() < drawingRect.height()
            || windowSz.width() < drawingRect.width()) {
        this->show();
        QSize outerSz = drawingRect.size();
        outerSz.scale(d->mapSz, d->mapSz, Qt::KeepAspectRatio);
        d->outerRect.setSize(outerSz);

        int width = windowSz.width();
        int height = windowSz.height();

        float widthDiff = (float) windowSz.width() / drawingRect.width();
        float heightDiff = (float) windowSz.height() / drawingRect.height();

        width = d->mapSz * widthDiff;
        height = d->mapSz * heightDiff;

        QSize innerSz = windowSz.scaled(width, height, Qt::KeepAspectRatio);
        d->innerRect.setSize(innerSz);

        float xSpeedDiff = (float) innerSz.width() / windowSz.width();
        float ySpeedDiff = (float) innerSz.height() / windowSz.height();

        int x = -drawingRect.left() * xSpeedDiff;
        int y = -drawingRect.top() * ySpeedDiff;

        d->innerRect.setTopLeft(QPoint(x, y));
        d->innerRect.setSize(innerSz);
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
