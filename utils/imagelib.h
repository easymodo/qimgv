#ifndef IMAGELIB_H
#define IMAGELIB_H

#include <QImage>
#include <QPainter>
#include <QPixmapCache>
#include <QDebug>
#include <memory>
//#include <FreeImagePlus.h>

class ImageLib {
    public:
        static QImage *rotated(const QImage *src, int grad);
        static QImage *rotated(std::shared_ptr<const QImage> src, int grad);

        static QImage *cropped(const QImage *src, QRect newRect);
        static QImage *cropped(std::shared_ptr<const QImage> src, QRect newRect);

        static QImage *flippedH(const QImage *src);
        static QImage *flippedH(std::shared_ptr<const QImage> src);

        static QImage *flippedV(const QImage *src);
        static QImage *flippedV(std::shared_ptr<const QImage> src);

        static QImage *scaled(const QImage *source, QSize destSize, int method);
        static QImage *scaled(std::shared_ptr<const QImage> source, QSize destSize, int method);

        static QImage *scaled_Qt(const QImage *source, QSize destSize, bool smooth);
        static QImage *scaled_Qt(std::shared_ptr<const QImage> source, QSize destSize, bool smooth);
        //static QImage *scale_FreeImage(const QImage *source, QSize destSize, FREE_IMAGE_FILTER filter);

        static std::unique_ptr<const QImage> exifRotated(std::unique_ptr<const QImage> src, int orientation);
        static std::unique_ptr<QImage> exifRotated(std::unique_ptr<QImage> src, int orientation);
};

#endif // IMAGELIB_H
