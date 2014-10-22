#include "imageloader.h"

ImageLoader::ImageLoader(DirectoryManager *dm) {
    cache = new ImageCache();
    dirManager = dm;
}

Image* ImageLoader::loadNext() {
    dirManager->next();
    Image *img = new Image(dirManager->getFile());
    loadFromCache(img);
    return img;
}

Image* ImageLoader::loadPrev() {
    dirManager->prev();
    Image *img = new Image(dirManager->getFile());
    loadFromCache(img);
    return img;
}

Image* ImageLoader::load(QString file) {
    dirManager->setFile(file);
    Image *img = new Image(dirManager->getFile());
    loadFromCache(img);
    return img;
}

void ImageLoader::loadFromCache(Image*& image)
{
    Image* found = cache->findImagePointer(image);
    if (!found)
    {
        cache->pushImage(image);
        qDebug() << "image not found, adding it to cache";
    }
    else
    {
        delete image;
        image = found;
        qDebug() << "image found" << image;
    }
}
