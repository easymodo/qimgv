#include "loaderrunnable.h"

LoaderRunnable::LoaderRunnable(QString _path) : path(_path) {
}

void LoaderRunnable::run() {
    std::shared_ptr<Image> image = ImageFactory::createImage(path);
    emit finished(image, path);
}
