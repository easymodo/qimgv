#ifndef IMAGELIB_H
#define IMAGELIB_H

#include <QImage>
#include <QPainter>
#include <cmath>
#include <ctime>
#include <QDebug>

enum fileType { STATIC, GIF };

class ImageLib {
public:
    static QPixmap* fastScale(const QImage* source, QSize destSize, bool smooth);
    static QPixmap* bilinearScale(const QImage *source, QSize destSize, bool smooth);
    static fileType guessType(QString path);
};

#endif // IMAGELIB_H

