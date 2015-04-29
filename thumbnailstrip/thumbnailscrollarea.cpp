#include "thumbnailscrollarea.h"

ThumbnailScrollArea::ThumbnailScrollArea(QWidget *parent) :
    QScrollArea(parent)
{
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollStep = 150;
    defaultHeight = 125;
    setFrameShape(QFrame::NoFrame);
    this->setMinimumHeight(defaultHeight);
    this->hide();
}

void ThumbnailScrollArea::wheelEvent(QWheelEvent *event) {

    if(event->angleDelta().ry() < 0) {
        horizontalScrollBar()->
                setValue(horizontalScrollBar()->value()+scrollStep);
    }
    else {
        horizontalScrollBar()->
                setValue(horizontalScrollBar()->value()-scrollStep);
    }
    event->accept();
}

void ThumbnailScrollArea::mouseMoveEvent(QMouseEvent* event) {
    event->accept();
}

void ThumbnailScrollArea::parentResized(QSize parentSize) {
    this->setGeometry( QRect(0, parentSize.height() - defaultHeight + 1,
                             parentSize.width(), defaultHeight) );
}

void ThumbnailScrollArea::leaveEvent(QEvent *event) {
    Q_UNUSED(event)
    this->hide();
}

ThumbnailScrollArea::~ThumbnailScrollArea() {

}

