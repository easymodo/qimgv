#include "clickablewidget.h"

ClickableWidget::ClickableWidget(QWidget *parent) : QWidget(parent) {
}

void ClickableWidget::mousePressEvent(QMouseEvent *event) {
    Q_UNUSED(event)
    emit pressed(event->pos());
    switch(event->button()) {
        case Qt::LeftButton:
            emit pressedLeft(event->pos());
            break;
        case Qt::RightButton:
            emit pressedRight(event->pos());
            break;
        case Qt::MiddleButton:
            emit pressedMiddle(event->pos());
            break;
        default:
            break;
    }
}
