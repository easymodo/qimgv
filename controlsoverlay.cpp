#include "controlsoverlay.h"

ControlsOverlay::ControlsOverlay(QWidget *parent) :
    QWidget(parent)
{
    setPalette(Qt::transparent);
    this->setGeometry(parentWidget()->size().rwidth()-60,0,60,20);
    pen.setColor(Qt::green);
    pen.setWidth(1);
}

void ControlsOverlay::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setPen(pen);
    painter.drawLine(QLine(4,15,15,15)); //minimize
    painter.drawRect(QRect(24,10,10,5)); //square
    painter.drawLine(QLine(44,4,54,14)); //cross 1
    painter.drawLine(QLine(44,15,54,5)); //cross 2
}

void ControlsOverlay::updateSize() {
    this->setGeometry(parentWidget()->size().rwidth()-60,0,60,20);
}

void ControlsOverlay::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        if(event->pos().rx() >=39)
            emit exitClicked();
        if(event->pos().rx() <39 && event->pos().rx() >=19)
            emit exitFullscreenClicked();
        if(event->pos().rx() <19 && event->pos().rx() >=0)
            emit minimizeClicked();
    }
}











