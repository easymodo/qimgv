#include "thumbnailview.h"

ThumbnailView::ThumbnailView() {
}

void ThumbnailView::wheelEvent(QWheelEvent *event) {
    event->ignore();
}

void ThumbnailView::mousePressEvent(QMouseEvent *event) {
    event->ignore();
}
