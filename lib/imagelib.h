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
    ImageLib();
    void fastScale(QPixmap *dest, const QImage* source, QSize destSize, bool smooth);
    void bilinearScale(QPixmap *dest, const QImage *source, QSize destSize, bool smooth);
    static fileType guessType(QString path);
};

#endif // IMAGELIB_H

