#include "imagelib.h"

#include <QFile>

ImageLib::ImageLib() {

}
/*
void ImageLib::fastScale(QPixmap* dest, QPixmap* source, QSize destSize, bool smooth) {
    dest->fill(qRgba(0,0,0,0));
    QPainter painter(dest);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, smooth);
    painter.drawImage(QRectF(QPointF(0,0),
                      destSize),
                      *source,
                      source->rect()
                      );
}
*/

void ImageLib::bilinearScale(QPixmap *dest, QPixmap* source, QSize destSize, bool smooth) {
    Qt::TransformationMode mode;
    if(smooth)
        mode = Qt::SmoothTransformation;
    else
        mode = Qt::FastTransformation;

    *dest = source->scaled(destSize.width(), destSize.height(), Qt::IgnoreAspectRatio, mode);
    delete source;

    /*
    dest->convertFromImage(source->scaled(destSize.width(),
                                          destSize.height(),
                                          Qt::IgnoreAspectRatio,
                                          mode)
                           );
                           */
}
