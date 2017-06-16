#include "loaderrunnable.h"

LoaderRunnable::LoaderRunnable(QString _path, int _index)
    : path(_path), index(_index)
{
}

void LoaderRunnable::run() {
    ImageFactory *factory = new ImageFactory();
    Image *image = factory->createImage(path);
    delete factory;
    emit finished(image, index);
}
