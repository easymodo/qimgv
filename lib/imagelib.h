#ifndef IMAGELIB_H
#define IMAGELIB_H

#include <QImage>
#include <QPainter>
#include <cmath>
#include <ctime>
#include <QDebug>

class ImageLib {
public:
    ImageLib();
    void fastScale(QPixmap *dest, QPixmap* source, QSize destSize, bool smooth);
    void bilinearScale(QPixmap *dest, QPixmap *source, QSize destSize, bool smooth);
};

#endif // IMAGELIB_H

