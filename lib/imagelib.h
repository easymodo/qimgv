#ifndef IMAGELIB_H
#define IMAGELIB_H

#include <QImage>
#include <QPainter>
#include <cmath>

QImage* fastScale(const QImage* source, QSize destSize, bool smooth);

QImage* bilinearScale(const QImage* source, QSize destSize);

#endif // IMAGELIB_H

