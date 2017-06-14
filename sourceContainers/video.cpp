#include "video.h"
#include <time.h>

//use this one
Video::Video(QString _path) {
    path = _path;
    loaded = false;
    fileInfo = new FileInfo(_path, this);
}

Video::Video(FileInfo *_info) {
    loaded = true;
    fileInfo = _info;
    path = fileInfo->filePath();
}

Video::~Video() {
    delete fileInfo;
    delete clip;
}

void Video::load() {
    QMutexLocker locker(&mutex);
    if(!fileInfo) {
        fileInfo = new FileInfo(path, this);
    }
    if(isLoaded()) {
        return;
    }
    clip = new Clip(path, fileInfo->fileExtension());
    loaded = true;
}

void Video::save(QString destinationPath) {
    if(isLoaded()) {
        lock();
        clip->save(destinationPath);
        unlock();
    }
}

void Video::save() {
    if(isLoaded()) {
        lock();
        clip->save(path);
        unlock();
    }
}

QPixmap *Video::generateThumbnail(int size, bool squared) {
    QString ffmpegExe = settings->ffmpegExecutable();
    if(ffmpegExe.isEmpty()) {
        return thumbnailStub();
    }

    QString filePath = settings->cacheDir() + "tmp_" + fileInfo->fileName();

    QString command = "\"" + ffmpegExe + "\"" + " -i " + "\"" + fileInfo->filePath() + "\"" +
                      " -r 1 -f image2 " + "\"" + filePath + "\"";
    QProcess process;
    process.start(command);
    bool success = process.waitForFinished(2000);
    process.close();
    if(success) {
        Qt::AspectRatioMode method = squared?(Qt::KeepAspectRatioByExpanding):(Qt::KeepAspectRatio);
        QPixmap *thumbnail = new QPixmap(size, size);
        QPixmap *tmp;
        tmp = new QPixmap(filePath, "JPG");
        *tmp = tmp->scaled(size * 2,
                           size * 2,
                           method,
                           Qt::FastTransformation)
               .scaled(size,
                       size,
                       method,
                       Qt::SmoothTransformation);

        QFile tmpFile(filePath);
        tmpFile.remove();

        if(squared) {
            QRect target(0, 0, size, size);
            target.moveCenter(tmp->rect().center());
            QPixmap *thumbnail = new QPixmap(size, size);
            *thumbnail = tmp->copy(target);
            delete tmp;
            return thumbnail;
        } else {
            return tmp;
        }
    } else {
        QFile tmpFile(filePath);
        if(tmpFile.exists()) {
            tmpFile.remove();
        }
        return thumbnailStub();
    }
}

QPixmap *Video::thumbnailStub() {
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
