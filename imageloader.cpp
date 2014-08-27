#include "imageloader.h"

ImageLoader::ImageLoader(DirectoryManager *dm) {
    cache = new ImageCache();
    dirManager = dm;
}

Image* ImageLoader::loadNext() {
    dirManager->next();
    Image *img = new Image(dirManager->getFile());
    return img;

}

Image* ImageLoader::loadPrev() {
    dirManager->prev();
    Image *img = new Image(dirManager->getFile());
    return img;
}

Image* ImageLoader::load(QString file) {
    dirManager->setFile(file);
    Image *img = new Image(dirManager->getFile());
    return img;
}
