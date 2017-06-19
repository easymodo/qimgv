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
        thumbImage = createThumbnailImage(&imgInfo, size, squared);
        // put in image info
        thumbImage->setText("originalWidth", QString::number(originalSize.width()));
        thumbImage->setText("originalHeight", QString::number(originalSize.height()));
        if(imgInfo.imageType() == ANIMATED) {
            thumbImage->setText("label", " [a]");
        } else if(imgInfo.imageType() == VIDEO) {
            thumbImage->setText("label", " [v]");
        }
        // save thumbnail if it makes sense
        if(originalSize.width() > size || originalSize.height() > size)
            thumbnailCache->saveThumbnail(thumbImage, thumbnailHash);
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

QImage* ThumbnailerRunnable::createThumbnailImage(ImageInfo *imgInfo, int size, bool squared) {
    ImageType type = imgInfo->imageType();
    Qt::AspectRatioMode method = squared?
                (Qt::KeepAspectRatioByExpanding):(Qt::KeepAspectRatio);
    QImage *full, *scaled;
    scaled = new QImage();
    ////////////////////////////////////////////////////////////////////////////
    ///////////////////////// TODO: fix this section
    if(type == VIDEO) {
        QString ffmpegExe = settings->ffmpegExecutable();
        if(ffmpegExe.isEmpty()) {
            full = videoThumbnailStub();
        } else {
            QString filePath = settings->cacheDir() + "tmp_" + imgInfo->fileName();
            QString command = "\"" + ffmpegExe + "\"" + " -i " + "\"" + imgInfo->filePath() + "\"" +
                              " -r 1 -f image2 " + "\"" + filePath + "\"";
            QProcess process;
            process.start(command);
            bool success = process.waitForFinished(2000);
            process.close();
            if(success) {
                full = new QImage(filePath, "JPG");
            } else {
                QFile tmpFile(filePath);
                if(tmpFile.exists()) {
                    tmpFile.remove();
                }
                return videoThumbnailStub();
            }
        }
    ////////////////////////////////////////////////////////////////////////////
    } else {
        full = new QImage(imgInfo->filePath(), imgInfo->extension());
    }
    *scaled = full->scaled(size * 2,
                           size * 2,
                           method,
                           Qt::FastTransformation)
                   .scaled(size,
                           size,
                           method,
                           Qt::SmoothTransformation);
    originalSize = full->size();
    delete full;
    if(squared) {
        QRect target(0, 0, size, size);
        target.moveCenter(scaled->rect().center());
        QImage *cropped = new QImage();
        *cropped = scaled->copy(target);
        delete scaled;
        return cropped;
    } else {
        return scaled;
    }
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
