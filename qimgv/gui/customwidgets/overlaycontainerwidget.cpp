#include "overlaycontainerwidget.h"

OverlayContainerWidget::OverlayContainerWidget(QWidget *parent) : QWidget(parent) {
}

void OverlayContainerWidget::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    emit resized(size());
}
