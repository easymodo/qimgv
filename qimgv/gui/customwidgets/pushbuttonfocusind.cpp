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
        QPainter p(this);
        p.setPen(QColor(98,98,100, 170));
        p.drawLine(10, height() - 5, width() - 10, height() - 5);
    }
}
