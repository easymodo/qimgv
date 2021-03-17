#include "pushbuttonfocusind.h"

PushButtonFocusInd::PushButtonFocusInd(QWidget *parent) : QPushButton(parent) {
    mIsHighlighted = false;
}

void PushButtonFocusInd::setHighlighted(bool mode) {
    mIsHighlighted = mode;
    update();
}

bool PushButtonFocusInd::isHighlighted() {
    return mIsHighlighted;
}

void PushButtonFocusInd::mousePressEvent(QMouseEvent *event) {
    if(event->button() == Qt::RightButton) {
        event->accept();
        emit rightPressed();
    } else {
        QPushButton::mousePressEvent(event);
    }
}

void PushButtonFocusInd::mouseReleaseEvent(QMouseEvent *event) {
    if(event->button() == Qt::RightButton) {
        event->accept();
        emit rightClicked();
    } else {
        QPushButton::mouseReleaseEvent(event);
    }
}

void PushButtonFocusInd::paintEvent(QPaintEvent *event) {
    QPushButton::paintEvent(event);
    if(mIsHighlighted) {
        QPainter painter(this);
        QPointF p1(4, 4);
        QPointF p2(13, 4);
        QPointF p3(4, 13);
        QPainterPath path(p1);
        path.lineTo(p2);
        path.lineTo(p3);
        painter.fillPath(path, settings->colorScheme().widget);
    }
}





