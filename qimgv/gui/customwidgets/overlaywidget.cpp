/*
 * Base class for overlay widgets.
 * It is not supposed to go into any kind of layout.
 * The widget will position itself depending on it's container size
 */

#include "overlaywidget.h"

OverlayWidget::OverlayWidget(OverlayContainerWidget *parent) : QWidget(parent) {
    this->setAccessibleName("OverlayWidget");
    connect(parent, SIGNAL(resized(QSize)), this, SLOT(onContainerResized(QSize)));
    hide();
}

QSize OverlayWidget::containerSize() {
    return container;
}

void OverlayWidget::setContainerSize(QSize container) {
    this->container = container;
    recalculateGeometry();
}

void OverlayWidget::onContainerResized(QSize size) {
    setContainerSize(size);
}

void OverlayWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event)
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
