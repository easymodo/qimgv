#include "mapoverlay.h"

mapOverlay::mapOverlay(QWidget *parent) :
    QWidget(parent)
{
    outer=QRect();
    outer.setX(0);
    outer.setY(0);
    inner=QRect();
    setPalette(Qt::transparent);
    this->setGeometry(parentWidget()->size().rwidth()-110,parentWidget()->size().rheight()-110,110,110);
    pen.setColor(Qt::green);
    pen.setWidth(1);
}

void mapOverlay::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setPen(pen);
    painter.drawRect(outer);
}

void mapOverlay::updateSize() {
    this->setGeometry(parentWidget()->size().rwidth()-110,parentWidget()->size().rheight()-110,110,110);
}

void mapOverlay::updateMap(QSize scr, QSize img, int srcx, int scry) {
    double ar1 = (double)scr.height()/scr.width();
    if(ar1==1) {
        outer.setWidth(100);
        outer.setHeight(100);
    }
    else {
        outer.setWidth(100);
        outer.setHeight(100);
    }
    this->repaint();
}
