#include "controlsoverlay.h"

ControlsOverlay::ControlsOverlay(QWidget *parent) :
    QWidget(parent) {
    setPalette(Qt::transparent);
    this->setGeometry(parentWidget()->size().rwidth() - 60, 0, 60, 20);
    pen.setColor(QColor(255, 255, 255, 255));
    pen.setWidth(1);
}

void ControlsOverlay::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event)

    QPainter painter(this);
    painter.fillRect(QRect(0, 0, 60, 20), QBrush(QColor(0, 0, 0, 80), Qt::SolidPattern));
    painter.setPen(pen);
    painter.drawLine(QLine(4, 15, 15, 15));    //minimize
    painter.drawRect(QRect(24, 10, 10, 5));    //square
    painter.drawLine(QLine(44, 5, 53, 14));    //cross 1
    painter.drawLine(QLine(44, 14, 53, 5));    //cross 2
}

void ControlsOverlay::updateSize(QSize containerSz) {
    this->setGeometry(containerSz.width() - 60, 0, 60, 20);
}

void ControlsOverlay::mouseReleaseEvent(QMouseEvent *event) {
    if(event->button() == Qt::LeftButton) {
        if(event->pos().rx() >= 39)
            emit exitClicked();
        if(event->pos().rx() < 39 && event->pos().rx() >= 19)
            emit exitFullscreenClicked();
        if(event->pos().rx() < 19 && event->pos().rx() >= 0)
            emit minimizeClicked();
    }
}
