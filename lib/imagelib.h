#ifndef IMAGELIB_H
#define IMAGELIB_H

#include <QImage>
#include <QPainter>
#include <QPixmapCache>
#include <QDebug>

class ImageLib {
public:
    ImageLib();
    void bilinearScale(QImage *dest, const QImage *source, QSize destSize, bool smooth);
//    void bicubicScale(QPixmap *outPixmap, const QImage *in, int destWidth, int destHeight);
};

#endif // IMAGELIB_H

