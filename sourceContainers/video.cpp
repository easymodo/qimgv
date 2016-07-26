#include "video.h"
#include <time.h>

//use this one
Video::Video(QString _path) {
    path = _path;
    loaded = false;
    info = new FileInfo(_path, this);
}

Video::Video(FileInfo *_info) {
    loaded = true;
    info = _info;
    path = info->getFilePath();
}

Video::~Video() {
    delete info;
    delete clip;
}

void Video::load() {
    QMutexLocker locker(&mutex);
    if(!info) {
        info = new FileInfo(path);
    }
    if(isLoaded()) {
        return;
    }
    clip = new Clip(path, info->getExtension());
    loaded = true;
}

void Video::save(QString destinationPath) {
    if(isLoaded()) {
        lock();
        clip->save(destinationPath, getExtension(destinationPath), 100);
        unlock();
    }
}

void Video::save() {
    if(isLoaded()) {
        lock();
        clip->save(path, info->getExtension(), 100);
        unlock();
    }
}

QPixmap *Video::generateThumbnail() {
    QString ffmpegExe = settings->ffmpegExecutable();
    if(ffmpegExe.isEmpty()) {
        return thumbnailStub();
    }

    QString filePath = settings->tempDir() + "tmp_" + info->getFileName();

    QString command = "\"" + ffmpegExe + "\"" + " -i " + "\"" + info->getFilePath() + "\"" +
                      " -r 1 -f image2 " + "\"" + filePath + "\"";
    QProcess process;
    process.start(command);
    bool success = process.waitForFinished(2000);
    process.close();
    if(success) {
        int size = settings->thumbnailSize();
        QPixmap *thumbnail = new QPixmap(size, size);
        QPixmap *tmp;
        tmp = new QPixmap(filePath, "JPG");
        *tmp = tmp->scaled(size * 2,
                           size * 2,
                           Qt::KeepAspectRatioByExpanding,
                           Qt::FastTransformation)
               .scaled(size,
                       size,
                       Qt::KeepAspectRatioByExpanding,
                       Qt::SmoothTransformation);

        QRect target(0, 0, size, size);
        target.moveCenter(tmp->rect().center());
        *thumbnail = tmp->copy(target);
        delete tmp;
        QFile tmpFile(filePath);
        tmpFile.remove();
        return thumbnail;
    } else {
        QFile tmpFile(filePath);
        if(tmpFile.exists()) {
            tmpFile.remove();
        }
        return thumbnailStub();
    }
}

QPixmap *Video::thumbnailStub() {
    int size = settings->thumbnailSize();
    QImage *img = new QImage(size, size, QImage::Format_ARGB32_Premultiplied);
    QPainter painter(img);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    QPixmap videoIcon(":/images/res/video_thumb.png");

    QBrush brush(QColor(0, 0, 0, 50));

    painter.fillRect(img->rect(), brush);
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

    QPoint topLeft((size - videoIcon.width()) / 2, (size - videoIcon.height()) / 2);
    painter.drawPixmap(topLeft, videoIcon);

    QPixmap *thumbnail = new QPixmap();
    *thumbnail = QPixmap::fromImage(*img).copy();
    return thumbnail;
}

QPixmap *Video::getPixmap() {
    qDebug() << "SOMETHING HAPPENED.";
    //TODO: find out some easy way to get frames from video source
    return NULL;
}

const QImage *Video::getImage() {
    qDebug() << "SOMETHING HAPPENED.";
    //TODO: find out some easy way to get frames from video source
    return NULL;
}

Clip *Video::getClip() {
    return clip;
}

int Video::height() {
    return isLoaded() ? clip->height() : 0;
}

int Video::width() {
    return isLoaded() ? clip->width() : 0;
}

QSize Video::size() {
    return isLoaded() ? clip->size() : QSize(0, 0);
}

void Video::rotate(int grad) {
    if (isLoaded()) {
        clip->rotate(grad);
    }
}

void Video::crop(QRect newRect) {
    if (isLoaded()) {
        clip->setFrame(newRect);
    }
}
