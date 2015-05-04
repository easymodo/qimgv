#include "thumbnaillabel.h"

ThumbnailLabel::ThumbnailLabel(QGraphicsPixmapItem *parent) :
    QGraphicsLayoutItem(),
    QGraphicsPixmapItem(parent),
    hovered(false),
    loaded(false),
    state(EMPTY),
    highlighted(false),
    border(4)
{
    highlightColor = new QColor(80, 80, 220);
    outlineColor = new QColor(Qt::black);
    setGraphicsItem(this);
    this->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    this->setOffset(QPointF(border, border));
//    setMouseTracking(true);
  //  this->setMinimumSize(100,100);
 //   this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

void ThumbnailLabel::setPixmap(const QPixmap &pixmap){
    QGraphicsPixmapItem::setPixmap(pixmap);
    loaded = true;
}

/*
void ThumbnailLabel::enterEvent(QEvent *event) {
    Q_UNUSED(event)
    hovered = true;
    update();
}

void ThumbnailLabel::leaveEvent(QEvent *event) {
    Q_UNUSED(event)
    hovered = false;
    update();
}
*/
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
        QPen pen;
        pen.setColor(*outlineColor);
        pen.setWidth(2);
        painter->setPen(pen);
        painter->drawRect(boundingRect());
    }
    QGraphicsPixmapItem::paint(painter,option,widget);
}

QSizeF ThumbnailLabel::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const
{
    switch (which) {
    case Qt::MinimumSize:
        return QSize(100, 100);
    case Qt::PreferredSize:
        return QSize(100, 100) + QSize(border*2, border*2);
    case Qt::MaximumSize:
        return QSize(100, 100) + QSize(border*2, border*2);
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

