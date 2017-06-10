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
    int time = clock();
    FileInfo *fileInfo = new FileInfo(path);
    Thumbnail *th = new Thumbnail();
    th->size = size;
    th->name = fileInfo->fileName();
    QString thumbnailHash = generateIdString();

    th->image = thumbnailCache->readThumbnail(thumbnailHash);
    if(th->image) {
        //qDebug() << "cached: " << clock() - time;
        delete fileInfo;
    } else {
        factory = new ImageFactory();
        Image* tempImage = factory->createImage(fileInfo);
        th->image = tempImage->generateThumbnail(size, squared);

        // save thumbnail if it is makes sense
        if(tempImage->width() > size || tempImage->height() > size)
            thumbnailCache->saveThumbnail(th->image, thumbnailHash);

        delete factory;
        delete tempImage;
        if(th->image->size() == QSize(0, 0)) {
            // TODO: wat, why would even it be zero size?
            // anyway, maybe show some error icon instead?
            delete th->image;
            th->image = new QPixmap(size, size);
            th->image->fill(QColor(0,0,0,0));
        }
        //qDebug() << "full load: " << clock() - time;
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

Thumbnailer::~Thumbnailer() {
}
