#include "loaderrunnable.h"

LoaderRunnable::LoaderRunnable(QString _path) : path(_path) {
}

void LoaderRunnable::run() {
    ImageFactory factory;
    Image *image = factory.createImage(path);
    emit finished(image);
}
