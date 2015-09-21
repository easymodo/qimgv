#include "loadhelper.h"

LoadHelper::LoadHelper(ImageCache *_cache, QThread *_mainThread, QObject *parent) :
    cache(_cache),
    loadTarget(0),
    mainThread(_mainThread),
    QObject(parent)
{

}

void LoadHelper::setTarget(int pos, QString _path) {
    loadTarget = pos;
    path = _path;
}

int LoadHelper::target() {
    return loadTarget;
}

void LoadHelper::doLoad() {
    int toLoad = loadTarget;
    //Image* tmp = new ImageStatic("/home/mitcher/tests/mix/3.png");
    //tmp->load();
    //cache->loadAt(toLoad);
    //this->thread()->msleep(1000);

    //qDebug()<< "loading " << path << " ....";
    ImageFactory *factory = new ImageFactory();
    cache->insert(factory->createImage("/home/mitcher/tests/mix/3.png"), toLoad);
    //cache->insert(factory->createImage(path), toLoad);
    delete factory;

    qDebug() << "load thread finished" << loadTarget;
    emit finished(toLoad);
}
