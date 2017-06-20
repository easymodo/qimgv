#include "thumbnailer.h"

ThumbnailerRunnable::ThumbnailerRunnable(ThumbnailCache* _thumbnailCache, QString _path, int _target, int _size, bool _squared) :
    path(_path),
    target(_target),
    size(_size),
    thumbnailCache(_thumbnailCache),
    squared(_squared)
{
}

void ThumbnailerRunnable::run() {
    ImageInfo imgInfo(path);
    Thumbnail *th = new Thumbnail();
    th->size = size;
    th->name = imgInfo.fileName();
    QString thumbnailHash = generateIdString();
    QImage *thumbImage = thumbnailCache->readThumbnail(thumbnailHash);
    if(!thumbImage) {
        thumbImage = createScaledThumbnail(&imgInfo, size, squared);
        // put in image info
        thumbImage->setText("originalWidth", QString::number(originalSize.width()));
        thumbImage->setText("originalHeight", QString::number(originalSize.height()));
        if(imgInfo.imageType() == ANIMATED) {
            thumbImage->setText("label", " [a]");
        } else if(imgInfo.imageType() == VIDEO) {
            thumbImage->setText("label", " [v]");
        }
        // save thumbnail if it makes sense
        //if(originalSize.width() > size || originalSize.height() > size)
        //    thumbnailCache->saveThumbnail(thumbImage, thumbnailHash);
    }
    th->image = new QPixmap(thumbImage->size());
    *th->image = QPixmap::fromImage(*thumbImage);
    // put info into Thumbnail object
    th->label = thumbImage->text("originalWidth") + "x" +
                thumbImage->text("originalWidth") +
                thumbImage->text("label");
    delete thumbImage;
    emit thumbnailReady(target, th);
}

QString ThumbnailerRunnable::generateIdString() {
    QString queryStr = path + QString::number(size);
    if(squared)
        queryStr.append("s");
    queryStr = QString("%1").arg(QString(QCryptographicHash::hash(queryStr.toUtf8(),QCryptographicHash::Md5).toHex()));
    return queryStr;
}

QImage* ThumbnailerRunnable::createScaledThumbnail(ImageInfo *imgInfo, int size, bool squared) {
    QImageReader reader;
    if(imgInfo->imageType() == VIDEO) {
     // todo
    } else {
        reader.setFileName(imgInfo->filePath());
    }
    reader.setFormat(imgInfo->extension());
    Qt::AspectRatioMode method = squared?
                (Qt::KeepAspectRatioByExpanding):(Qt::KeepAspectRatio);
    QSize scaledSize = reader.size().scaled(size, size, method);
    reader.setScaledSize(scaledSize);
    if(squared) {
        QRect clip(0, 0, size, size);
        QRect scaledRect(QPoint(0,0), scaledSize);
        clip.moveCenter(scaledRect.center());
        reader.setScaledClipRect(clip);
    }
    if(!reader.supportsOption(QImageIOHandler::Size))
        qDebug() << imgInfo->fileName() << " - ImageFormat does not support Size option.";
    else
        originalSize = reader.size();
    QImage *scaled = new QImage(reader.read());
    return scaled;
}

// wtf is happening in this method?
QImage* ThumbnailerRunnable::videoThumbnailStub() {
    int size = settings->mainPanelSize();
    QImage *thumbnail = new QImage(size, size, QImage::Format_ARGB32_Premultiplied);
    QPainter painter(thumbnail);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    QImage videoIcon(":/res/icons/video_thumb.png");

    QBrush brush(QColor(0, 0, 0, 50));

    painter.fillRect(thumbnail->rect(), brush);
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

    QPoint topLeft((size - videoIcon.width()) / 2, (size - videoIcon.height()) / 2);
    painter.drawImage(topLeft, videoIcon);

    return thumbnail;
}

ThumbnailerRunnable::~ThumbnailerRunnable() {
}
