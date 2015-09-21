#include "loadhelper.h"

LoadHelper::LoadHelper(ImageCache *_cache, QThread *_mainThread) :
    cache(_cache),
    loadTarget(0),
    mainThread(_mainThread)
{

}

void LoadHelper::setTarget(int pos, QString _path) {
    mutex.lock();
    loadTarget = pos;
    path = _path;
    mutex.unlock();
}

int LoadHelper::target() {
    return loadTarget;
}

void LoadHelper::doLoad() {
    mutex.lock();
    int targetLocal = loadTarget;
    QString pathLocal = path;
    mutex.unlock();
    //Image* tmp = new ImageStatic("/home/mitcher/tests/mix/3.png");
    //tmp->load();
    //cache->loadAt(toLoad);
    //this->thread()->msleep(1000);
    qDebug() << "worker::loadId " << QThread::currentThread();
    ImageFactory *factory = new ImageFactory();
    Image *img = factory->createImage(pathLocal);
    img->moveToThread(mainThread);
    qDebug() << "moving to " << mainThread;
    //cache->insert(factory->createImage("/home/mitcher/tests/mix/3.png"), toLoad)
    cache->insert(img, targetLocal);
    delete factory;

    qDebug() << "load thread finished: " << targetLocal;
    emit finished(targetLocal);
}
