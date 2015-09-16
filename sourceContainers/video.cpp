#include "video.h"
#include <time.h>

//use this one
Video::Video(QString _path) {
    path = _path;
    loaded = true;
    type = GIF;
    extension = NULL;
    info=NULL;
}

Video::Video(FileInfo *_info) {
    loaded = true;
    type = GIF;
    extension = "VIDEO";
    info=_info;
    path=info->getFilePath();
}

Video::~Video() {
}

void Video::load() {
}

void Video::unload() {
}

void Video::save(QString destinationPath) {
}

void Video::save() {
}

QPixmap* Video::generateThumbnail() {

    QString ffmpegExe = globalSettings->ffmpegExecutable();
    if(ffmpegExe.isEmpty()) {
        return thumbnailStub();
    }


    QString filePath = globalSettings->tempDir() + "tmp_" + info->getFileName();

    QString command = "\""+ffmpegExe+"\""+" -i "+"\""+info->getFilePath()+"\""+
            " -r 1 -f image2 "+"\""+filePath+"\"";
    QProcess process;
    process.start(command);
    process.waitForFinished();
    process.close();

    int size = globalSettings->thumbnailSize();
    QPixmap *thumbnail = new QPixmap(size, size);
    QPixmap *tmp;
    tmp = new QPixmap(filePath, "JPG");
    tmp->save("C:/testThumb.jpg", "JPG", 95);
    *tmp = tmp->scaled(size*2,
                       size*2,
                       Qt::KeepAspectRatioByExpanding,
                       Qt::FastTransformation)
               .scaled(size,
                       size,
                       Qt::KeepAspectRatioByExpanding,
                       Qt::SmoothTransformation);

    QRect target(0, 0, size,size);
    target.moveCenter(tmp->rect().center());
    *thumbnail = tmp->copy(target);
    delete tmp;
    return thumbnail;
}

QPixmap* Video::thumbnailStub() {
    int size = globalSettings->thumbnailSize();
    QImage *img = new QImage(size, size, QImage::Format_ARGB32_Premultiplied);
    QPainter painter(img);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    QPixmap videoIcon(":/images/res/video_thumb.png");

    QBrush brush(QColor(0, 0, 0, 50));

    painter.fillRect(img->rect(), brush);
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

    QPoint topLeft((size-videoIcon.width())/2, (size-videoIcon.height())/2);
    painter.drawPixmap(topLeft, videoIcon);

    QPixmap* thumbnail = new QPixmap();
    *thumbnail = QPixmap::fromImage(*img).copy();
    return thumbnail;
}

QPixmap* Video::getPixmap() {
    qDebug() << "Something bad happened in Video::getPixmap().";
    //TODO: find out some easy way to get frames from video source
    return NULL;
}

int Video::height() {
    return 0;
}

int Video::width() {
    return 0;
}

QSize Video::size() {
    return QSize(0,0);
}

QString Video::filePath() {
    return info->getFilePath();
}

QImage* Video::rotated(int grad) {

}

void Video::rotate(int grad) {

}

void Video::crop(QRect newRect) {

}
