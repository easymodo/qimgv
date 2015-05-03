#include "thumbnaillabel.h"

ThumbnailLabel::ThumbnailLabel(QWidget *parent) :
    QLabel(parent),
    hovered(false),
    loaded(false),
    state(EMPTY)
{
    setMouseTracking(true);
    this->setMinimumSize(100,100);
    this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

void ThumbnailLabel::mousePressEvent ( QMouseEvent * event ) {
    Q_UNUSED(event)
    emit clicked(this);
    event->accept();
}

void ThumbnailLabel::setPixmap(const QPixmap &pixmap){
    QLabel::setPixmap(pixmap);
    loaded = true;
}

void ThumbnailLabel::paintEvent(QPaintEvent* event) {
    QLabel::paintEvent(event);
    if(hovered) {
        QPainter painter(this);
        painter.fillRect(rect(), QBrush(QColor(50,50,100,40)));
    }
}

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

// rect relative to parent
QRect ThumbnailLabel::relativeRect() {
    return QRect(mapToParent(this->rect().topLeft()), mapToParent(this->rect().bottomRight()));
}

ThumbnailLabel::~ThumbnailLabel() {

}

