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
    
    QPen innerPen, outerPen, outlinePen;
    float xSpeedDiff, ySpeedDiff;
    QPen penInner, penOuter;
    QRectF outerRect, innerRect;
    QRectF windowRect, drawingRect;
    MapOverlay* q;
    QPropertyAnimation* opacityAnimation, *transitionAnimation;
    float innerOffset;
    float opacity;
    
    int size;
    MapOverlay::Location location;
    int margin;
};

MapOverlay::MapOverlayPrivate::MapOverlayPrivate(MapOverlay* qq)
    : q(qq), size(100), opacity(1.0f), innerOffset(-1), margin(10)
{
    outlinePen.setColor(QColor(40,40,40,255));
    innerPen.setColor(QColor(160,160,160,150));
    outerPen.setColor(QColor(80,80,80,150));
    
    location = MapOverlay::RightBottom;
}

MapOverlay::MapOverlayPrivate::~MapOverlayPrivate()
{
    delete opacityAnimation;
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
    d->opacityAnimation = new QPropertyAnimation(this, "opacity");
    d->opacityAnimation->setEasingCurve(QEasingCurve::InQuad);
    d->opacityAnimation->setDuration(300);
    
    d->transitionAnimation = new QPropertyAnimation(this, "y");
    d->transitionAnimation->setDuration(300);
    d->transitionAnimation->setEasingCurve(QEasingCurve::OutCubic);
    
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

void MapOverlay::animateVisible(bool isVisible)
{
    if (QWidget::isVisible() == isVisible) // already in this state
        return;

    d->opacityAnimation->setEndValue(1.0f * isVisible);
    
    switch (location())
    {
        case MapOverlay::LeftTop:
        case MapOverlay::RightTop:
            if (isVisible)
            {
                d->transitionAnimation->setStartValue(0);
                d->transitionAnimation->setEndValue(margin());
            }
            else
            {
                d->transitionAnimation->setStartValue(margin());
                d->transitionAnimation->setEndValue(0);
            }
            break;
        case MapOverlay::RightBottom:
        case MapOverlay::LeftBottom:
            int h = parentWidget()->height();
            int offset = d->outerRect.height() + margin();
            
            if (isVisible)
            {
                d->transitionAnimation->setStartValue(h);
                d->transitionAnimation->setEndValue(h - offset);
            }
            else
            {
                d->transitionAnimation->setStartValue(h - offset);
                d->transitionAnimation->setEndValue(h);
            }
            break;
    }
    
    d->opacityAnimation->start();
    d->transitionAnimation->start();
}

void MapOverlay::resize(int size)
{
    QWidget::resize(size, size);
}

void MapOverlay::setY(int y)
{
    move(x(), y);
}

int MapOverlay::y() const
{
    return QWidget::y();
}

void MapOverlay::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QBrush outerBrush(QColor(80, 80, 80, 200), Qt::SolidPattern);
    QBrush innerBrush(QColor(230,230,230,150), Qt::SolidPattern);
    
    painter.setOpacity(d->opacity);

    painter.setPen(d->innerPen);
    painter.fillRect(d->innerRect, innerBrush);

    painter.setPen(d->outerPen);
    painter.fillRect(d->outerRect, outerBrush);

    painter.setPen(d->outlinePen);
    painter.drawRect(d->outerRect);
    painter.drawRect(d->innerRect);
}

void MapOverlay::updatePosition()
{
    QRect parentRect = parentWidget()->rect();
    
    int x = 0, y = 0;
    switch (location())
    {
        case MapOverlay::LeftTop:
            x = parentRect.left() + margin();
            y = parentRect.top() + margin();
            break;
        case MapOverlay::RightTop:
            x = parentRect.right() - (margin() + d->outerRect.width());
            y = parentRect.top() + margin();
            break;
        case MapOverlay::RightBottom:
            x = parentRect.right() - (margin() + d->outerRect.width());
            y = parentRect.bottom() - (margin() + d->outerRect.height());
            break;
        case MapOverlay::LeftBottom:
            x = parentRect.left() + margin();
            y = parentRect.bottom() - (margin() + d->outerRect.height());
            break;
    }

    /**
     * Save extra space for outer rect border
     * one pixel for right and bottom sides
     */
    setGeometry(x, y, d->size + 1, d->size + 1);
}

bool contains(const QRectF& real, const QRectF& expected)
{
    return real.width() <= expected.width() && real.height() <= expected.height();
}

void MapOverlay::updateMap(const QRectF& drawingRect)
{
    if (!isEnabled())
        return;
    
    QRectF windowRect = parentWidget()->rect();
    
    bool needToBeHidden = !contains(drawingRect, windowRect);
    animateVisible(needToBeHidden);
    
    /**
     * Always calculate this first for properly map location
     */
    QSizeF outerSz = drawingRect.size();
    outerSz.scale(d->size, d->size, Qt::KeepAspectRatio);
    d->outerRect.setSize(outerSz);
    
    d->windowRect = windowRect;
    d->drawingRect = drawingRect;
    
    float aspect = outerSz.width() / drawingRect.width();
    
    float innerWidth = std::min((float) windowRect.width() * aspect,
                                (float) outerSz.width());
    
    float innerHeight = std::min((float) windowRect.height() * aspect,
                                 (float) outerSz.height());
    
    QSizeF innerSz(innerWidth, innerHeight);
    d->innerRect.setSize(innerSz);
    
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

void MapOverlay::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    updatePosition();
}

int MapOverlay::size() const
{
    return d->size;
}

MapOverlay::Location MapOverlay::location() const
{
    return d->location;
}

void MapOverlay::setLocation(MapOverlay::Location loc)
{
    d->location = loc;
}

int MapOverlay::margin() const
{
    return d->margin;
}

void MapOverlay::setMargin(int margin)
{
    d->margin = margin;
}
