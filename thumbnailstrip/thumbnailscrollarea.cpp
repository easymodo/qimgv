#include "thumbnailscrollarea.h"

ThumbnailScrollArea::ThumbnailScrollArea() {
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollStep = 100;
    this->setMinimumHeight(120);
}

void ThumbnailScrollArea::wheelEvent(QWheelEvent *event) {

    if(event->angleDelta().ry() < 0) {
        horizontalScrollBar()->setValue(horizontalScrollBar()->value()+scrollStep);
    }
    else {
        horizontalScrollBar()->setValue(horizontalScrollBar()->value()-scrollStep);
    }
    event->accept();
}



ThumbnailScrollArea::~ThumbnailScrollArea()
{

}

