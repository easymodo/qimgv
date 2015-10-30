#include "clickablewidget.h"

ClickableWidget::ClickableWidget(QWidget *parent) : QWidget(parent) {
}

void ClickableWidget::mousePressEvent(QMouseEvent *event) {
    Q_UNUSED(event)
    emit pressed(event->pos());
}
