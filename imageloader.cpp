#include "imageloader.h"

ImageLoader::ImageLoader(DirectoryManager *_dm) {
    cache = new ImageCache();
    dm = _dm;
    loadDelayEnabled = false;
    readSettings();
    connect(globalSettings, SIGNAL(settingsChanged()),
            this, SLOT(readSettings()));
}

void ImageLoader::load(QString path) {
    load(dm->setFile(path));
}

void ImageLoader::load(FileInfo* file) {
    Image *img = new Image(file);
    setCurrentImg(img);
    QtConcurrent::run(this, &ImageLoader::load_thread, img);
}

void ImageLoader::loadNext()
{
    load(dm->next());
}

void ImageLoader::loadPrev()
{
    load(dm->prev());
}

void ImageLoader::load_thread(Image* img)
{
    emit loadStarted();
    if(loadDelayEnabled) QThread::msleep(35);
    if(isCurrent(img)) {
        mutex2.lock();
        qDebug() << "loadStart: " << img->getName();
        Image* found = cache->findImage(img);
        if(!found) {
            img->loadImage();
            cache->cacheImageForced(img);
        }
        else {
            delete img;
            img = found;
        }
        img->setInUse(true);
        mutex2.unlock();
        emit startPreload();
        emit loadFinished(img);
    }
}

void ImageLoader::preloadNearest() {
    preload(dm->peekNext());
    preload(dm->peekPrev());
}

void ImageLoader::preload(FileInfo *file) {
    Image* img = new Image(file);
    if(!cache->findImage(img)) { // not found; preloading
        QtConcurrent::run(this, &ImageLoader::preload_thread, img);
    }
}

void ImageLoader::preload_thread(Image* img) {
    lock();
    qDebug() << "PreloadStart: " << img->getName();
    img->loadImage();
    if(!cache->cacheImageForced(img)) {
        delete img;
        img = NULL;
    }
    else {
        img->moveToThread(this->thread()); // important
    }
    unlock();
}

Image *ImageLoader::getCurrentImg() const
{
    return currentImg;
}

void ImageLoader::setCurrentImg(Image *value)
{
    lock();
    currentImg = value;
    unlock();
}

bool ImageLoader::isCurrent(Image* img) {
    lock();
    bool flag = (img == currentImg);
    unlock();
    return flag;
}

void ImageLoader::readSettings() {
    loadDelayEnabled = globalSettings->s.value("loadDelay", "false").toBool();
    if(globalSettings->s.value("usePreloader", true).toBool()) {
        connect(this, SIGNAL(startPreload()),
                this, SLOT(preloadNearest()));
    }
    else {
        disconnect(this, SIGNAL(startPreload()),
                   this, SLOT(preloadNearest()));
    }
}

void ImageLoader::lock()
{
    mutex.lock();
}

void ImageLoader::unlock()
{
    mutex.unlock();
}
