#include "thumbnaillabel.h"

ThumbnailLabel::ThumbnailLabel(QGraphicsPixmapItem *parent) :
    QGraphicsLayoutItem(),
    QGraphicsPixmapItem(parent),
    hovered(false),
    loaded(false),
    state(EMPTY),
    highlighted(false),
    borderW(3),
    borderH(5),
    thumbnailSize(100)
{
    setGraphicsItem(this);
    this->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    this->setOffset(QPointF(borderW, borderH));
    this->setShapeMode(QGraphicsPixmapItem::BoundingRectShape);

    highlightRect.setTopLeft(QPointF(borderW, 0));
    highlightRect.setBottomRight(QPointF(borderW+thumbnailSize, borderH));
    shadowRect.setTopLeft(QPointF(borderW, borderH));
    shadowRect.setBottomRight(QPointF(borderW+thumbnailSize, borderH+10));
    highlightColor = new QColor(76, 191, 10);
    outlineColor = new QColor(Qt::black);
    shadowGradient = new QLinearGradient(shadowRect.topLeft(),
                                         shadowRect.bottomLeft());
    shadowGradient->setColorAt(0, QColor(0,0,0,170));
    shadowGradient->setColorAt(1, QColor(0,0,0,0));
}

void ThumbnailLabel::setPixmap(const QPixmap &pixmap){
    QGraphicsPixmapItem::setPixmap(pixmap);
    loaded = true;
}

void ThumbnailLabel::setHighlighted(bool x) {
    bool toRepaint = (highlighted==x);
    highlighted = x;
    if(toRepaint) {
        this->update(boundingRect());
    }
}

bool ThumbnailLabel::isHighlighted() {
    return highlighted;
}

void ThumbnailLabel::setOpacityAnimated(qreal amount, int speed) {
    if(speed == 0.0) {
        setOpacity(amount);
        return;
    }
    QPropertyAnimation *anim = new QPropertyAnimation(this, "opacity");
    anim->setDuration(speed);
    anim->setStartValue(this->opacity());
    anim->setEndValue(amount);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}
void ThumbnailLabel::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QGraphicsPixmapItem::paint(painter,option,widget);
    if(isHighlighted()) {
        painter->fillRect(highlightRect, *highlightColor);
        painter->fillRect(shadowRect, *shadowGradient);

    }
}

QSizeF ThumbnailLabel::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const
{
    switch (which) {
    case Qt::MinimumSize:
    case Qt::PreferredSize:
        return QSize(thumbnailSize, thumbnailSize)
               + QSize(borderW*2, borderH*2);
    case Qt::MaximumSize:
        return QSize(thumbnailSize, thumbnailSize)
               + QSize(borderW*2, borderH*2);
    default:
        break;
    }
    return constraint;
}

void ThumbnailLabel::setGeometry(const QRectF &rect) {
    prepareGeometryChange();
    QGraphicsLayoutItem::setGeometry(rect);
    setPos(rect.topLeft());
}

QRectF ThumbnailLabel::boundingRect() const
{
    return QRectF(QPointF(0,0), geometry().size());
}

ThumbnailLabel::~ThumbnailLabel() {

}

