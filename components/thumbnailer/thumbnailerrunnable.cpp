#include "thumbnailerrunnable.h"

ThumbnailerRunnable::ThumbnailerRunnable(ThumbnailCache* _thumbnailCache, QString _path, int _size, bool _squared) :
    path(_path),
    size(_size),
    thumbnailCache(_thumbnailCache),
    squared(_squared)
{
}

void ThumbnailerRunnable::run() {
    emit taskStart(path);
    ImageInfo imgInfo(path);
    Thumbnail *th = new Thumbnail();
    th->size = size;
    th->name = imgInfo.fileName();
    QString thumbnailHash = generateIdString();
    QImage *thumbImage = NULL;
    if(settings->useThumbnailCache())
        thumbImage = thumbnailCache->readThumbnail(thumbnailHash);
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
        if(settings->useThumbnailCache()) {
        // save thumbnail if it makes sense
            if(originalSize.width() > size || originalSize.height() > size)
                thumbnailCache->saveThumbnail(thumbImage, thumbnailHash);
        }
    }
    th->image = new QPixmap(thumbImage->size());
    *th->image = QPixmap::fromImage(*thumbImage);
    // put info into Thumbnail object
    th->label = thumbImage->text("originalWidth") + "x" +
                thumbImage->text("originalHeight") +
                thumbImage->text("label");
    delete thumbImage;
    emit taskEnd(th, path);
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
    QString filePath;
    if(imgInfo->imageType() == VIDEO) {
        QString mpv = settings->mpvBinary();
        if(!mpv.isEmpty()) {
            filePath = settings->tempDir() + imgInfo->baseName() + ".png";
            QString command = "\"" + mpv + "\"" + " --start=30% --frames=1 --aid=no --sid=no --no-config --load-scripts=no --no-terminal --o=\"" + filePath + "\" \"" + imgInfo->filePath() + "\"";
            QProcess process;
            process.start(command);
            bool success = process.waitForFinished(3000);
            process.close();
        }
    } else {
        filePath = imgInfo->filePath();
    }
    reader.setFileName(filePath);
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
    // remove temporary file in video case
    if(imgInfo->imageType() == VIDEO) {
        QFile tmpFile(filePath);
        tmpFile.remove();
    }
    return scaled;
}

ThumbnailerRunnable::~ThumbnailerRunnable() {
}
