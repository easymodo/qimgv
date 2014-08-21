#include "controlsoverlay.h"

controlsOverlay::controlsOverlay(QWidget *parent) :
    QWidget(parent)
{
    setPalette(Qt::transparent);
    this->setGeometry(parentWidget()->size().rwidth()-60,0,60,20);
    pen.setColor(Qt::green);
    pen.setWidth(1);
}

void controlsOverlay::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setPen(pen);
    //painter.drawRect(QRect(0,-1,this->size().rwidth(), this->size().rheight()));
    //painter.drawLine(QLine(19,0,19,19));
    //painter.drawLine(QLine(39,0,39,19));
    painter.drawLine(QLine(4,15,15,15)); //minimize
    painter.drawRect(QRect(24,10,10,5)); //square

    painter.drawLine(QLine(44,4,54,14)); //cross 1
    painter.drawLine(QLine(44,15,54,5)); //cross 2
}

void controlsOverlay::updateSize() {
    this->setGeometry(parentWidget()->size().rwidth()-60,0,60,20);
}

void controlsOverlay::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        if(event->pos().rx() >=39)
            emit exitClicked();
        if(event->pos().rx() <39 && event->pos().rx() >=19)
            emit exitFullscreenClicked();
        if(event->pos().rx() <19 && event->pos().rx() >=0)
            emit minimizeClicked();
    }
}











