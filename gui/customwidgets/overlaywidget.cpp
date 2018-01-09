/*
 * Base class for floating widgets.
 * It is not supposed to go into any kind of layout.
 * Instead, you specify the container size and widget will decide
 * where to draw itself within that area.
 * Drawn over the parent widget.
 */

#include "overlaywidget.h"

OverlayWidget::OverlayWidget(QWidget *parent) : QWidget(parent) {
    this->setAccessibleName("OverlayWidget");
}

QSize OverlayWidget::containerSize() {
    return container;
}

void OverlayWidget::setContainerSize(QSize container) {
    this->container = container;
    recalculateGeometry();
}

void OverlayWidget::paintEvent(QPaintEvent *event) {
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
