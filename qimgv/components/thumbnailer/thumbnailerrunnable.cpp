#include "thumbnailerrunnable.h"

ThumbnailerRunnable::ThumbnailerRunnable(ThumbnailCache* _cache, QString _path, int _size, bool _crop, bool _force) :
    path(_path),
    size(_size),
    crop(_crop),
    force(_force),
    cache(_cache)
{
}

void ThumbnailerRunnable::run() {
    emit taskStart(path, size);
    std::shared_ptr<Thumbnail> thumbnail = generate(cache, path, size, crop, force);
    emit taskEnd(thumbnail, path);
}

QString ThumbnailerRunnable::generateIdString(QString path, int size, bool crop) {
    QString queryStr = path + QString::number(size);
    if(crop)
        queryStr.append("s");
    queryStr = QString("%1").arg(QString(QCryptographicHash::hash(queryStr.toUtf8(),QCryptographicHash::Md5).toHex()));
    return queryStr;
}

std::shared_ptr<Thumbnail> ThumbnailerRunnable::generate(ThumbnailCache* cache, QString path, int size, bool crop, bool force) {
    DocumentInfo imgInfo(path);
    QString thumbnailId = generateIdString(path, size, crop);
    std::unique_ptr<QImage> image;

    if(!force && cache)
        image.reset(cache->readThumbnail(thumbnailId));

    if(!image) {
        std::pair<QImage*, QSize> pair;
        if(imgInfo.type() == VIDEO)
            pair = createVideoThumbnail(path, size, crop);
        else
            pair = createThumbnail(imgInfo.filePath(), imgInfo.format().toStdString().c_str(), size, crop);
        image.reset(pair.first);
        QSize originalSize = pair.second;

        image = ImageLib::exifRotated(std::move(image), imgInfo.exifOrientation());

        // put in image info
        image.get()->setText("originalWidth", QString::number(originalSize.width()));
        image.get()->setText("originalHeight", QString::number(originalSize.height()));

        if(imgInfo.type() == ANIMATED)
            image.get()->setText("label", " [a]");
        else if(imgInfo.type() == VIDEO)
            image.get()->setText("label", " [v]");

        if(cache) {
            // save thumbnail if it makes sense
            // FIXME: avoid too much i/o
            if(originalSize.width() > size || originalSize.height() > size)
                cache->saveThumbnail(image.get(), thumbnailId);
        }
    }
    auto && tmpPixmap = new QPixmap(image->size());
    *tmpPixmap = QPixmap::fromImage(*image);
    tmpPixmap->setDevicePixelRatio(qApp->devicePixelRatio());

    QString label;
    if(tmpPixmap->width() == 0) {
        label = "error";
    } else  {
        // put info into Thumbnail object
        label = image.get()->text("originalWidth") +
                "x" +
                image.get()->text("originalHeight") +
                image.get()->text("label");
    }
    std::shared_ptr<QPixmap> pixmapPtr(tmpPixmap);
    std::shared_ptr<Thumbnail> thumbnail(new Thumbnail(imgInfo.fileName(), label, size, pixmapPtr));
    return thumbnail;
}

ThumbnailerRunnable::~ThumbnailerRunnable() {
}

std::pair<QImage*, QSize> ThumbnailerRunnable::createThumbnail(QUrl path, const char *format, int size, bool squared) {
    QImageReader reader;
    reader.setFileName(path.toString());
    reader.setFormat(format);
    Qt::AspectRatioMode ARMode = squared?
                (Qt::KeepAspectRatioByExpanding):(Qt::KeepAspectRatio);
    QImage *result = nullptr;
    QSize originalSize;
    if(reader.supportsOption(QImageIOHandler::Size)) { // resize during via QImageReader
        QSize scaledSize = reader.size().scaled(size, size, ARMode);
        reader.setScaledSize(scaledSize);
        if(squared) {
            QRect clip(0, 0, size, size);
            QRect scaledRect(QPoint(0,0), scaledSize);
            clip.moveCenter(scaledRect.center());
            reader.setScaledClipRect(clip);
        }
        originalSize = reader.size();
        result = new QImage(reader.read());
    } else { // scale & crop manually
        QImage *fullSize = new QImage(reader.read());
        originalSize = fullSize->size();
        QSize scaledSize = fullSize->size().scaled(size, size, ARMode);
        if(squared) {
            QRect clip(0, 0, size, size);
            QRect scaledRect(QPoint(0,0), scaledSize);
            clip.moveCenter(scaledRect.center());
            QImage scaled = QImage(fullSize->scaled(scaledSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
            result = ImageLib::cropped(&scaled, clip);
        } else {
            result = new QImage(fullSize->scaled(scaledSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
        }
        delete fullSize;
    }
    // force reader to close file so it can be deleted later
    reader.setFileName("");

    return std::make_pair(result, originalSize);
}

std::pair<QImage*, QSize> ThumbnailerRunnable::createVideoThumbnail(QUrl path, int size, bool squared) {
    QImageReader reader;
    QString tmpFilePath = settings->tmpDir() + path.fileName() + ".png";
    QString tmpFilePathEsc = tmpFilePath;
    tmpFilePathEsc.replace("%", "%%");
    QString command = "\"" + settings->mpvBinary() + "\" --start=30% --frames=1 --aid=no --sid=no --no-config --load-scripts=no --no-terminal --o=\"" + tmpFilePathEsc + "\" \"" + path.toString() + "\"";
    QProcess process;
    process.start(command);
    process.waitForFinished(8000);
    process.close();

    reader.setFileName(tmpFilePath);
    reader.setFormat("png");
    Qt::AspectRatioMode ARMode = squared?
                (Qt::KeepAspectRatioByExpanding):(Qt::KeepAspectRatio);
    QImage *result = nullptr;

    // scale & crop
    QSize scaledSize = reader.size().scaled(size, size, ARMode);
    reader.setScaledSize(scaledSize);
    if(squared) {
        QRect clip(0, 0, size, size);
        QRect scaledRect(QPoint(0,0), scaledSize);
        clip.moveCenter(scaledRect.center());
        reader.setScaledClipRect(clip);
    }
    QSize originalSize = reader.size();
    result = new QImage(reader.read());

    // force reader to close file so it can be deleted later
    reader.setFileName("");

    // remove temporary file
    QFile tmpFile(tmpFilePath);
    tmpFile.remove();

    return std::make_pair(result, originalSize);
}
