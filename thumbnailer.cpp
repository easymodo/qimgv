#include "thumbnailer.h"

Thumbnailer::Thumbnailer(ThumbnailCache* _thumbnailCache, QString _path, int _target, int _size, bool _squared) :
    path(_path),
    target(_target),
    size(_size),
    thumbnailCache(_thumbnailCache),
    squared(_squared)
{

}

void Thumbnailer::run() {
    ImageInfo *fileInfo = new ImageInfo(path);
    Thumbnail *th = new Thumbnail();
    th->size = size;
    th->name = fileInfo->fileName();
    QString thumbnailHash = generateIdString();

    th->image = thumbnailCache->readThumbnail(thumbnailHash);
    if(th->image) {
        delete fileInfo;
    } else {
        factory = new ImageFactory();
        Image* tempImage = factory->createImage(fileInfo);
        // !!!!! TODO: load in constructor right away. Fix cache if needed.
        tempImage->load();
        th->image = createThumbnailImage(tempImage, size, squared);

        // save thumbnail if it is makes sense
        if(tempImage->width() > size || tempImage->height() > size)
            thumbnailCache->saveThumbnail(th->image, thumbnailHash);

        delete factory;
        delete tempImage;
        if(th->image->size() == QSize(0, 0)) {
            // TODO: wat, why would even it be zero size?
            delete th->image;
            th->image = new QPixmap(size, size);
            th->image->fill(QColor(0,0,0,0));
        }
    }

    //qDebug() << "########### thumbnail " << target << " loaded in: " << clock() - time << "     thread:  " << this->thread();
    // TODO: implement this info in FileInfo class
    //th->label.append(QString::number(tempImage->width()));
    //th->label.append("x");
    //th->label.append(QString::number(tempImage->height()));
    /*
    if(tempImage->type() == ANIMATED) {
        th->label.append(" [a]");
    } else if(tempImage->type() == VIDEO) {
        th->label.append(" [v]");
    }
    */
    emit thumbnailReady(target, th);
}

QString Thumbnailer::generateIdString() {
    QString queryStr = path + QString::number(size);
    if(squared)
        queryStr.append("s");
    queryStr = QString("%1").arg(QString(QCryptographicHash::hash(queryStr.toUtf8(),QCryptographicHash::Md5).toHex()));
    return queryStr;
}

QPixmap* Thumbnailer::createThumbnailImage(Image *img, int size, bool square) {
    ImageType type = img->type();
    Qt::AspectRatioMode method = square?(Qt::KeepAspectRatioByExpanding):(Qt::KeepAspectRatio);
    QPixmap *full, *scaled, *squared;
    if(square) {
        squared = new QPixmap(size, size);
    }
    scaled = new QPixmap(size, size);
    if(type == VIDEO) {
        QString ffmpegExe = settings->ffmpegExecutable();
        if(ffmpegExe.isEmpty()) {
            full = videoThumbnailStub();
        } else {
            QString filePath = settings->cacheDir() + "tmp_" + img->info()->fileName();
            QString command = "\"" + ffmpegExe + "\"" + " -i " + "\"" + img->info()->filePath() + "\"" +
                              " -r 1 -f image2 " + "\"" + filePath + "\"";
            QProcess process;
            process.start(command);
            bool success = process.waitForFinished(2000);
            process.close();
            if(success) {
                full = new QPixmap(filePath, "JPG");
            } else {
                QFile tmpFile(filePath);
                if(tmpFile.exists()) {
                    tmpFile.remove();
                }
                return videoThumbnailStub();
            }
        }
    } else {
        full = new QPixmap(img->info()->filePath(), img->info()->extension());
    }
    *scaled = full->scaled(size * 2,
                       size * 2,
                       method,
                       Qt::FastTransformation)
               .scaled(size,
                       size,
                       method,
                       Qt::SmoothTransformation);
    delete full;
    if(squared) {
        QRect target(0, 0, size, size);
        target.moveCenter(scaled->rect().center());
        QPixmap *cropped = new QPixmap(size, size);
        *cropped = scaled->copy(target);
        delete scaled;
        return cropped;
    } else {
        return scaled;
    }
}

// wtf is happening in this method?
QPixmap* Thumbnailer::videoThumbnailStub() {
    int size = settings->mainPanelSize();
    QImage *img = new QImage(size, size, QImage::Format_ARGB32_Premultiplied);
    QPainter painter(img);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    QPixmap videoIcon(":/res/video_thumb.png");

    QBrush brush(QColor(0, 0, 0, 50));

    painter.fillRect(img->rect(), brush);
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

    QPoint topLeft((size - videoIcon.width()) / 2, (size - videoIcon.height()) / 2);
    painter.drawPixmap(topLeft, videoIcon);

    QPixmap *thumbnail = new QPixmap();
    *thumbnail = QPixmap::fromImage(*img).copy();
    return thumbnail;
}

Thumbnailer::~Thumbnailer() {
}
