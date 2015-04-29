#include "imagelib.h"

#include <QFile>

QPixmap* ImageLib::fastScale(const QImage* source, QSize destSize, bool smooth) {
    QPixmap* pixmap = new QPixmap(destSize);
    pixmap->fill(qRgba(0,0,0,0));
    QPainter painter(pixmap);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, smooth);
    painter.drawImage(QRectF(QPointF(0,0),
                      destSize),
                      *source,
                      source->rect()
                      );
    return pixmap;
}

QPixmap* ImageLib::bilinearScale(const QImage* source, QSize destSize, bool smooth) {
    QPixmap* pixmap = new QPixmap(destSize);
    Qt::TransformationMode mode;
    if(smooth)
        mode = Qt::SmoothTransformation;
    else
        mode = Qt::FastTransformation;
    int time = clock();
    QImage temp = source->scaled(destSize.width(),
                                 destSize.height(),
                                 Qt::IgnoreAspectRatio,
                                 mode);
    time = clock();
    pixmap->convertFromImage(temp);
    return pixmap;
}

fileType ImageLib::guessType(QString path) {
    QFile file(path);
    file.open(QIODevice::ReadOnly);
    //read first 2 bytes to determine file format
    QByteArray startingBytes= file.read(2).toHex();
    file.close();

    if(startingBytes=="4749") {
        return GIF;
    }
    else {
        return STATIC;
    }
}
