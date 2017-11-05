#ifndef IMAGELIB_H
#define IMAGELIB_H

#include <QImage>
#include <QPainter>
#include <QPixmapCache>
#include <QDebug>
#include <FreeImagePlus.h>

class ImageLib {
public:
    ImageLib();
    void scale(QImage *dest, const QImage *source, QSize destSize, bool smooth);
    void scalehq(QImage *dest, const QImage *source, QSize destSize);
//    void bicubicScale(QPixmap *outPixmap, const QImage *in, int destWidth, int destHeight);
};

#endif // IMAGELIB_H

