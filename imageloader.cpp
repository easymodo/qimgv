#include "imageloader.h"

ImageLoader::ImageLoader(DirectoryManager *dm) {
    cache = new ImageCache();
    dirManager = dm;
    notCached = NULL;
}

Image* ImageLoader::loadNext() {
    dirManager->next();
    Image *img = new Image(dirManager->getFile());
    loadImage(img);
    preload(dirManager->peekNext()); // move to thread later.. maybe
    return img;
}

Image* ImageLoader::loadPrev() {
    dirManager->prev();
    Image *img = new Image(dirManager->getFile());
    loadImage(img);
    preload(dirManager->peekPrev()); // move to thread
    return img;
}

Image* ImageLoader::load(QString file) {
    dirManager->setFile(file);
    Image *img = new Image(dirManager->getFile());
    loadImage(img);
    preload(dirManager->peekNext()); // move to thread
    preload(dirManager->peekPrev()); // move to thread
    return img;
}

void ImageLoader::preload(FileInfo info) {
    qDebug() << "LOADER: PRELOADING = " << info.getName();
    Image *img = new Image(info);
    if (!cache->imageIsCached(img))
    {
        img->loadImage();
        if(!cache->pushImage(img)) {
            deleteLastImage();
            notCached = img;
        }
        qDebug() << "LOADER: image preloaded";
    }
}

void ImageLoader::loadImage(Image*& image)
{
    qDebug() << "LOADER: opening " << image->getName();
    Image* found = cache->findImagePointer(image);
    if (!found)
    {
        image->loadImage();
        image->setInUse(true);
        if(!cache->pushImage(image)) {
            deleteLastImage();
            notCached = image;
            qDebug() << "LOADER: image not found, loading";
        }
    }
    else
    {
        delete image;
        image = found;
        qDebug() << "LOADER: image found" << image;
    }
}

void ImageLoader::deleteLastImage() {
    if(notCached && !notCached->isInUse()) {// && !cache->imageIsCached(img)) {
        delete notCached;
        notCached = NULL;
    }
}
