#include "video.h"
#include <time.h>

Video::Video(QString _path)
    : Image(_path)
{
    load();
}

Video::Video(std::unique_ptr<DocumentInfo> _info)
    : Image(std::move(_info))
{
    load();
}

Video::~Video() {
    delete clip;
}

void Video::load() {
    if(isLoaded()) {
        return;
    }
    clip = new Clip(mPath, mDocInfo->extension());
    mLoaded = true;
}

bool Video::save(QString destPath) {
    //clip->save(destPath);
    qDebug() << "Saving video is unsupported.";
    return false;
}

bool Video::save() {
    //clip->save(path);
    qDebug() << "Saving video is unsupported.";
    return false;
}

std::unique_ptr<QPixmap> Video::getPixmap() {
    qDebug() << "SOMETHING HAPPENED.";
    //TODO: find out some easy way to get frames from video source
    return nullptr;
}

const QImage *Video::getImage() {
    qDebug() << "SOMETHING HAPPENED.";
    //TODO: find out some easy way to get frames from video source
    return nullptr;
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
