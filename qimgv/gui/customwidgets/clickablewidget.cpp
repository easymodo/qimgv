#include "clickablewidget.h"

ClickableWidget::ClickableWidget(QWidget *parent) : QWidget(parent) {
}

void ClickableWidget::mousePressEvent(QMouseEvent *event) {
    QWidget::mousePressEvent(event);
    emit pressed();
}

void ClickableWidget::mouseReleaseEvent(QMouseEvent *event) {
    QWidget::mouseReleaseEvent(event);
    emit clicked();
}
