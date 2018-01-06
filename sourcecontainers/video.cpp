#include "video.h"
#include <time.h>

Video::Video(QString _path) {
    path = _path;
    loaded = false;
    imageInfo = new ImageInfo(_path);
    load();
}

Video::~Video() {
    delete clip;
}

void Video::load() {
    if(isLoaded()) {
        return;
    }
    if(!imageInfo) {
        imageInfo = new ImageInfo(path);
    }
    clip = new Clip(path, imageInfo->extension());
    loaded = true;
}

void Video::save(QString destPath) {
    clip->save(destPath);
}

void Video::save() {
    clip->save(path);
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
    return clip->height();
}

int Video::width() {
    return clip->width();
}

QSize Video::size() {
    return clip->size();
}
