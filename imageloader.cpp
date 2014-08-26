#include "imageloader.h"

ImageLoader::ImageLoader(DirectoryManager *dm) :
    QObject(parent)
{
    cache = new ImageCache();
    dirManager = dm;
}

Image* ImageLoader::loadNext() {

}

Image* ImageLoader::loadPrev() {

}

Image* ImageLoader::load() {
    Image *img = new Image(dirManager->getFile());
    return img;
}
