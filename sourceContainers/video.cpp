#include "video.h"
#include <time.h>

//use this one
Video::Video(QString _path) {
    path = _path;
    loaded = true;
    info = new FileInfo(_path, this);
    resolution = QSize(0,0);
}

Video::Video(FileInfo *_info) {
    loaded = true;
    info = _info;
    path = info->getFilePath();
}

Video::~Video() {
    delete info;
}

void Video::load() {
}

void Video::save(QString destinationPath) {
    Q_UNUSED(destinationPath)
}

void Video::save() {
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
    process.waitForFinished();
    process.close();

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

void Video::updateResolution() {
    QString ffmpegExe = settings->ffmpegExecutable();
    if(ffmpegExe.isEmpty()) {
        return;
    }

    QString command = "\"" + ffmpegExe + "\"" + " -i " + "\"" + filePath() + "\"";
    QProcess process;
    process.start(command);
    process.waitForFinished(100);
    QByteArray out = process.readAllStandardError();
    process.close();

    QRegExp expResolution("[0-9]+x[0-9]+");
    QRegExp expWidth("[0-9]+\\B");
    QRegExp expHeight("\\B+[0-9]+$");
    expResolution.indexIn(out);
    QString res = expResolution.cap();
    expWidth.indexIn(res);
    expHeight.indexIn(res);
    QString wt = expWidth.cap();
    QString ht = expHeight.cap();

    resolution = QSize(wt.toInt(), ht.toInt());
    qDebug() << wt + "x" + ht;
}

QPixmap *Video::getPixmap() {
    qDebug() << "Something bad happened in Video::getPixmap().";
    //TODO: find out some easy way to get frames from video source
    return NULL;
}

int Video::height() {
    if (resolution.height()==0){
        return size().height();
    }
    return resolution.height();
}

int Video::width() {
    if (resolution.width()==0){
        return size().width();
    }
    return resolution.width();
}

QSize Video::size() {
    if (resolution==QSize(0,0)){
        updateResolution();
    }
    return resolution;
}

QString Video::filePath() {
    return info->getFilePath();
}

QImage *Video::rotated(int grad) {
    Q_UNUSED(grad)
}

void Video::rotate(int grad) {
    Q_UNUSED(grad)

}

void Video::crop(QRect newRect) {
    Q_UNUSED(newRect)

}
