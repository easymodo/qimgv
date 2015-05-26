#include "thumbnaillabel.h"

ThumbnailLabel::ThumbnailLabel(QGraphicsPixmapItem *parent) :
    QGraphicsLayoutItem(),
    QGraphicsPixmapItem(parent),
    hovered(false),
    loaded(false),
    state(EMPTY),
    highlighted(false),
    borderW(3),
    borderH(4)
{
    highlightColor = new QColor(144, 209, 75);
    outlineColor = new QColor(Qt::black);
    setGraphicsItem(this);
    this->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    this->setOffset(QPointF(borderW, borderH));
    this->setShapeMode(QGraphicsPixmapItem::BoundingRectShape);
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
void ThumbnailLabel::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if(isHighlighted()) {
        painter->fillRect(boundingRect(), *highlightColor);
    }
    QGraphicsPixmapItem::paint(painter,option,widget);
}

QSizeF ThumbnailLabel::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const
{
    switch (which) {
    case Qt::MinimumSize:
    case Qt::PreferredSize:
        return QSize(100, 100) + QSize(borderW*2, borderH*2);
    case Qt::MaximumSize:
        return QSize(100, 100) + QSize(borderW*2, borderH*2);
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

