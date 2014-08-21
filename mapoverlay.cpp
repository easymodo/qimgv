#include "mapoverlay.h"

mapOverlay::mapOverlay(QWidget *parent) :
    QWidget(parent)
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
    mapSize = 100;
    mapMargin = 10;
    outer = new QRect();
    outer->setX(0);
    outer->setY(0);
    inner = new QRect();
    setPalette(Qt::transparent);
    this->setGeometry(parentWidget()->size().rwidth()-mapSize-mapMargin,parentWidget()->size().rheight()-mapSize-mapMargin,mapSize+mapMargin,mapSize+mapMargin);
}

void mapOverlay::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.fillRect(*outer, QBrush(QColor(230,230,230,150), Qt::SolidPattern));
    painter.fillRect(*inner, QBrush(QColor(100,100,100,200), Qt::SolidPattern));
    painter.setPen(QPen(QColor(150,150,150,200)));
    painter.drawRect(*inner);
}

void mapOverlay::updateSize() {
    this->setGeometry(parentWidget()->size().rwidth()-mapSize-mapMargin,parentWidget()->size().rheight()-mapSize-mapMargin,mapSize+mapMargin,mapSize+mapMargin);
}

void mapOverlay::updateMap(QSize scr, QSize img, double scrx, double scry) {

    double xdiff=1.0,ydiff=1.0;
    if(img.width()>scr.width())
        xdiff=(double)scr.width()/img.width();
    if(img.height()>scr.height())
        ydiff=(double)scr.height()/img.height();
    if(xdiff==1 && ydiff==1) {
        this->hide();
    }
    else {
        this->show();
        double ar1 = (double)img.height()/img.width();
        if(ar1==1) {
            outer->setCoords(0,0,mapSize,mapSize);
        }
        else if (ar1>1){
            outer->setCoords(mapSize-mapSize/ar1,0,mapSize,mapSize);
        }
        else {
            outer->setCoords(0,mapSize-mapSize*ar1,mapSize,mapSize);
        }

        // I dont know
        // Leave me alone
        double tmp1, tmp2, tmp3, tmp4;
        tmp1=xdiff*outer->width();
        tmp2=ydiff*outer->height();
        tmp3=tmp1+outer->x()+(outer->width()-tmp1)*scrx;
        tmp4=tmp2+outer->y()+(outer->height()-tmp2)*scry;

        inner->setCoords(outer->x()+(outer->width()-tmp1)*scrx+1,
                         outer->y()+(outer->height()-tmp2)*scry+1,
                         tmp3-3,
                         tmp4-3);
        this->repaint();
    }
}
