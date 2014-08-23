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
    painter.setPen(QPen(QColor(150,150,150,200)));
    painter.fillRect(*outer, QBrush(QColor(230,230,230,150), Qt::SolidPattern));
    painter.drawRect(*outer);
    painter.fillRect(*inner, QBrush(QColor(80,80,80,200), Qt::SolidPattern));
    painter.drawRect(*inner);
}

void mapOverlay::updateSize() {
    this->setGeometry(parentWidget()->size().rwidth()-mapSize-mapMargin,parentWidget()->size().rheight()-mapSize-mapMargin,mapSize+mapMargin,mapSize+mapMargin);
}

/* calculates outer(image) and inner(view area) squares
 * trust me, it just does
 */
void mapOverlay::updateMap(double widthDifferenceRatio,
                           double heightDifferenceRatio,
                           double viewportPositionX,
                           double viewportPositionY,
                           double imageAspectRatio) {
    if(imageAspectRatio == 1) {
        outer->setCoords(0, 0, mapSize, mapSize);
    }
    else if (imageAspectRatio > 1) {
        outer->setCoords(mapSize - mapSize / imageAspectRatio, 0, mapSize, mapSize);
    }
    else {
        outer->setCoords(0, mapSize-mapSize * imageAspectRatio, mapSize, mapSize);
    }
    double tmp1, tmp2, tmp3, tmp4;
    tmp1 = widthDifferenceRatio * outer->width();
    tmp2 = heightDifferenceRatio * outer->height();
    tmp3 = tmp1 + outer->x() + (outer->width() - tmp1) * viewportPositionX;
    tmp4 = tmp2 + outer->y() + (outer->height() - tmp2) * viewportPositionY;
    inner->setCoords(outer->x() + (outer->width() - tmp1) * viewportPositionX + 1,
                     outer->y() + (outer->height() - tmp2) * viewportPositionY + 1,
                     tmp3 - 2,
                     tmp4 - 2);
    this->update();
}
