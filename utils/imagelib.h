#ifndef IMAGELIB_H
#define IMAGELIB_H

#include <QImage>
#include <QPainter>
#include <QPixmapCache>
#include <QDebug>
//#include <FreeImagePlus.h>

class ImageLib {
    public:
        static QImage *rotate(const QImage *src, int grad);
        static QImage *crop(const QImage *src, QRect newRect);
        static QImage *scale(const QImage *source, QSize destSize, int method);
        static QImage *scale_Qt(const QImage *source, QSize destSize, bool smooth);
        //static QImage *scale_FreeImage(const QImage *source, QSize destSize, FREE_IMAGE_FILTER filter);
};

#endif // IMAGELIB_H

