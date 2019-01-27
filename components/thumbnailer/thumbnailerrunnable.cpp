#include "thumbnailerrunnable.h"

ThumbnailerRunnable::ThumbnailerRunnable(ThumbnailCache* _thumbnailCache, QString _path, int _size, bool _squared) :
    path(_path),
    size(_size),
    squared(_squared),
    thumbnailCache(_thumbnailCache)
{
}

void ThumbnailerRunnable::run() {
    // TODO: replace raw pointers with c++11 ones
    emit taskStart(path, size);
    DocumentInfo imgInfo(path);
    QString tmpName = imgInfo.fileName();
    QString thumbnailId = generateIdString();
    std::unique_ptr<QImage> image;
    if(settings->useThumbnailCache())
        image.reset(thumbnailCache->readThumbnail(thumbnailId));
    if(!image) {
        image.reset(createThumbnailImage(&imgInfo, size, squared));
        image = ImageLib::exifRotated(std::move(image), imgInfo.exifOrientation());
        // put in image info
        image.get()->setText("originalWidth", QString::number(originalSize.width()));
        image.get()->setText("originalHeight", QString::number(originalSize.height()));
        if(imgInfo.type() == ANIMATED) {
            image.get()->setText("label", " [a]");
        } else if(imgInfo.type() == VIDEO) {
            image.get()->setText("label", " [v]");
        }
        if(settings->useThumbnailCache()) {
            // save thumbnail if it makes sense
            // FIXME: avoid too much i/o
            if(originalSize.width() > size || originalSize.height() > size)
                thumbnailCache->saveThumbnail(image.get(), thumbnailId);
        }
    }
    QPixmap *tmpPixmap = new QPixmap(image->size());
    *tmpPixmap = QPixmap::fromImage(*image);
    tmpPixmap->setDevicePixelRatio(qApp->devicePixelRatio());

    QString tmpLabel;
    if(tmpPixmap->width() == 0) {
        tmpLabel = "error";
    } else  {
        // put info into Thumbnail object
        tmpLabel = image.get()->text("originalWidth") + "x" +
                   image.get()->text("originalHeight") +
                   image.get()->text("label");
    }
    std::shared_ptr<const QPixmap> pixmapPtr(tmpPixmap);
    std::shared_ptr<Thumbnail> thumbnail(new Thumbnail(tmpName, tmpLabel, size, pixmapPtr));
    emit taskEnd(thumbnail, path);
}

QString ThumbnailerRunnable::generateIdString() {
    QString queryStr = path + QString::number(size);
    if(squared)
        queryStr.append("s");
    queryStr = QString("%1").arg(QString(QCryptographicHash::hash(queryStr.toUtf8(),QCryptographicHash::Md5).toHex()));
    return queryStr;
}

QImage* ThumbnailerRunnable::createThumbnailImage(DocumentInfo *imgInfo, int size, bool squared) {
    QImageReader reader;
    QString filePath;
    if(imgInfo->type() == VIDEO) {
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
    if(imgInfo->type() == VIDEO) {
        QFile tmpFile(filePath);
        tmpFile.remove();
    }
    return scaled;
}

ThumbnailerRunnable::~ThumbnailerRunnable() {
}
