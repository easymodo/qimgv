#include "imagelib.h"

QImage* fastScale(const QImage* source, QSize destSize, bool smooth) {
    //bool smoothEnabled;

    QImage* image = new QImage(destSize,QImage::Format_ARGB32_Premultiplied);
    image->fill(qRgba(0,0,0,0));
    QPainter painter(image);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, smooth);
    painter.drawImage(QRectF(QPointF(0,0),
                      destSize),
                      *source,
                      source->rect()
                      );
    return image;
}

QImage* bilinearScale(const QImage* source, QSize destSize) {
    QImage* image = new QImage(destSize,QImage::Format_ARGB32_Premultiplied);
    *image = source->scaled(destSize.width(),
                                             destSize.height(),
                                             Qt::IgnoreAspectRatio,
                                             Qt::SmoothTransformation);
    return image;
}




