#include "imageloader.h"

ImageLoader::ImageLoader(DirectoryManager *dm) {
    cache = new ImageCache();
    dirManager = dm;
}

Image* ImageLoader::loadNext() {

}

Image* ImageLoader::loadPrev() {

}

Image* ImageLoader::load(QString file) {
    dirManager->setFile(file);
    Image *img = new Image(dirManager->getFile());
    return img;
}
