#include "mapoverlay.h"
#include <QPropertyAnimation>
#include "settings.h"

class MapOverlay::MapOverlayPrivate : public QObject
{
public:
    MapOverlayPrivate(MapOverlay* qq);
    ~MapOverlayPrivate();
    void moveInnerWidget(float x, float y);
    void moveMainImage(float xDist, float yDist);
    
    QPen innerPen, outerPen;
    float xSpeedDiff, ySpeedDiff;
    QPen penInner, penOuter;
    QRectF outerRect, innerRect;
    QRectF windowRect, drawingRect;
    MapOverlay* q;
    QPropertyAnimation* animation;
    float innerOffset;
    
    bool isAutoVisible;
    float opacity;
    
    int size;
    MapOverlay::Location location;
    int margin;
};

MapOverlay::MapOverlayPrivate::MapOverlayPrivate(MapOverlay* qq) : q(qq), size(100),
isAutoVisible(true), opacity(0.0f), innerOffset(-1), margin(10)
{
    innerPen.setColor(QColor(150,150,150,200));
    outerPen.setColor(QColor(150,150,150,200));
    
    location = MapOverlay::RightBottom;
}

MapOverlay::MapOverlayPrivate::~MapOverlayPrivate()
{
    delete animation;
}

void MapOverlay::MapOverlayPrivate::moveInnerWidget(float x, float y)
{
    if (x + innerRect.width() > outerRect.right())
        x = outerRect.right() - innerRect.width();
    
    if (x < 0)
        x = 0;
    
    if (y + innerRect.height() > outerRect.bottom())
        y = outerRect.bottom() - innerRect.height();
    
    if (y < 0)
        y = 0;
    
    innerRect.moveTo(QPointF(x, y));
    q->update();
}

void MapOverlay::MapOverlayPrivate::moveMainImage(float xPos, float yPos)
{
    float x = xPos - (innerRect.width() / 2);
    float y = yPos - (innerRect.height() / 2);
    
    moveInnerWidget(x, y);
    
    x /= -xSpeedDiff;
    y /= -ySpeedDiff;
    
    // Check limits;
    float invisibleX = windowRect.width() - drawingRect.width();
    float invisibleY = windowRect.height() - drawingRect.height();
    
    if (x < invisibleX) x = invisibleX;
    if (x > 0) x = 0;
    
    if (y < invisibleY) y = invisibleY;
    if (y > 0) y = 0;
    
    emit q->positionChanged(x, y);
}

MapOverlay::MapOverlay(QWidget *parent) : QWidget(parent),
d(new MapOverlayPrivate(this))
{
    d->animation = new QPropertyAnimation(this, "opacity");
    d->animation->setDuration(300);
    
    setCursor(Qt::OpenHandCursor);
 }

MapOverlay::~MapOverlay()
{
    delete d;
}

QSizeF MapOverlay::inner() const
{
    return d->innerRect.size();
}

QSizeF MapOverlay::outer() const
{
    return d->outerRect.size();
}

float MapOverlay::opacity() const
{
    return d->opacity;
}

void MapOverlay::setOpacity(float opacity)
{
    d->opacity = opacity;
    update();
    setVisible(opacity != 0.0f);
}

void MapOverlay::animateVisible(bool visible)
{
    if (isVisible() == visible) // already in this state
        return;
    
    d->animation->setEndValue(1.0f * visible);
    d->animation->start();
}

void MapOverlay::resize(int size)
{
    QWidget::resize(size, size);
}

void MapOverlay::setAutoVisible(bool state)
{
    d->isAutoVisible = state;
}

void MapOverlay::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QBrush outerBrush(QColor(80, 80, 80, 200), Qt::SolidPattern);
    QBrush innerBrush(QColor(230,230,230,150), Qt::SolidPattern);
    
    painter.setOpacity(d->opacity);
    
    painter.setPen(d->innerPen);
    painter.fillRect(d->innerRect, innerBrush);
    painter.drawRect(d->innerRect);
    
    painter.setPen(d->outerPen);
    painter.fillRect(d->outerRect, outerBrush);
    painter.drawRect(d->outerRect);
}

void MapOverlay::parentResized(int width, int height)
{
    int x = 0, y = 0;
    switch (location())
    {
        case MapOverlay::LeftTop:
            x = margin();
            y = margin();
            break;
        case MapOverlay::RightTop:
            x = width - (margin() + size());
            y = margin();
            break;
        case MapOverlay::RightBottom:
            x = width - (margin() + size());
            y = height - (margin() + size());
            break;
        case MapOverlay::LeftBottom:
            x = width + margin();
            y = height - (margin() + size());
            break;
    }
    
    /**
     * Save extra space for outer rect border
     * one pixel for right and bottom sides
     */
    setGeometry(x, y, d->size + 1, d->size + 1);
}

void MapOverlay::updateMap(const QRectF& windowRect, const QRectF& drawingRect)
{
    if (d->isAutoVisible)
    {
        bool needToBeHidden = !windowRect.contains(drawingRect);
        animateVisible(needToBeHidden);
    }
    
    d->windowRect = windowRect;
    d->drawingRect = drawingRect;
    
    QSizeF outerSz = drawingRect.size();
    outerSz.scale(d->size, d->size, Qt::KeepAspectRatio);
    d->outerRect.setSize(outerSz);
    
    float aspect = outerSz.width() / drawingRect.width();
    
    float innerWidth = std::min((float) windowRect.width() * aspect, (float) outerSz.width());
    float innerHeight = std::min((float) windowRect.height() * aspect, (float) outerSz.height());
    
    QSizeF innerSz(innerWidth, innerHeight);
    d->innerRect.setSize(innerSz);
    
    qDebug() << "outer" << d->outerRect.toAlignedRect() << "inner" << d->innerRect.toAlignedRect() << geometry();
    
    d->xSpeedDiff = innerSz.width() / windowRect.width();
    d->ySpeedDiff = innerSz.height() / windowRect.height();
    
    float x = (float) -drawingRect.left() * d->xSpeedDiff;
    float y = (float) -drawingRect.top() * d->ySpeedDiff;
    
    d->moveInnerWidget(x, y);
    update();
}

void MapOverlay::mousePressEvent(QMouseEvent* event)
{
    QWidget::mousePressEvent(event);
    setCursor(Qt::ClosedHandCursor);
    d->moveMainImage(event->x(), event->y());
    event->accept();
}

void MapOverlay::mouseMoveEvent(QMouseEvent* event)
{
    QWidget::mouseMoveEvent(event);
    d->moveMainImage(event->x(), event->y());
    event->accept();
}

void MapOverlay::mouseReleaseEvent(QMouseEvent* event)
{
    QWidget::mouseReleaseEvent(event);
    setCursor(Qt::OpenHandCursor);
}

int MapOverlay::size() const
{
    return d->size;
}

MapOverlay::Location MapOverlay::location() const
{
    return d->location;
}

void MapOverlay::setLocation(MapOverlay::Location l)
{
    d->location = l;
}

int MapOverlay::margin() const
{
    return d->margin;
}

void MapOverlay::setMargin(int margin)
{
    d->margin = margin;
}
