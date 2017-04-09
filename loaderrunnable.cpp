#include "loaderrunnable.h"

LoaderRunnable::LoaderRunnable(QString _path, int _index, QThread *_mainThread)
    : path(_path), index(_index), mainThread(_mainThread)
{
}

void LoaderRunnable::run() {
    //qDebug() << "runnable: " << path << "  > " << index << "thread: " << QThread::currentThread() << mainThread;
    ImageFactory *factory = new ImageFactory();
    Image *image = factory->createImage(path);
    delete factory;
    image->moveToThread(mainThread);
    emit finished(image, index);
}
