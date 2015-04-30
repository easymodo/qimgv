#include "imagelib.h"

#include <QFile>

ImageLib::ImageLib() {

}

void ImageLib::fastScale(QPixmap* dest, const QImage* source, QSize destSize, bool smooth) {
    dest->fill(qRgba(0,0,0,0));
    QPainter painter(dest);
    qDebug() << source;
    painter.setRenderHint(QPainter::SmoothPixmapTransform, smooth);
    painter.drawImage(QRectF(QPointF(0,0),
                      destSize),
                      *source,
                      source->rect()
                      );
}

void ImageLib::bilinearScale(QPixmap *dest, const QImage* source, QSize destSize, bool smooth) {
    Qt::TransformationMode mode;
    if(smooth)
        mode = Qt::SmoothTransformation;
    else
        mode = Qt::FastTransformation;
    dest->convertFromImage(source->scaled(destSize.width(),
                                          destSize.height(),
                                          Qt::IgnoreAspectRatio,
                                          mode)
                           );
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
