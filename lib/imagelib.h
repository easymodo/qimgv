#ifndef IMAGELIB_H
#define IMAGELIB_H

#include <QImage>
#include <QPainter>
#include <QDebug>

class ImageLib {
public:
    ImageLib();
    void bilinearScale(QPixmap *dest, QPixmap *source, QSize destSize, bool smooth);
};

#endif // IMAGELIB_H

