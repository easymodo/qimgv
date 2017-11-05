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
    QImage *scale(const QImage *source, QSize destSize, int method);
    QImage *scale_Qt(const QImage *source, QSize destSize, bool smooth);
    QImage *scale_FreeImage(const QImage *source, QSize destSize, FREE_IMAGE_FILTER filter);
};

#endif // IMAGELIB_H

