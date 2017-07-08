#include "cache2.h"

 Cache2::Cache2() {
    sem = new QSemaphore(1);
}

void Cache2::lock() {
    sem->acquire();
}

void Cache2::unlock() {
    sem->release();
}

bool Cache2::contains(QString name) {
    return images.contains(name);
}

bool Cache2::insert(QString name, Image* img) {
    if(images.contains(name)) {
        qDebug() << "Cache::insert - " << name << " already exists in map, ignoring";
        return false;
    }
    images.insert(name, new CacheItem(img));
    return true;
}

bool Cache2::insert(QString name, QString path) {
    if(!images.contains(name)) {
        return insert(name, ImageFactory::createImage(path));
    } else {
        qDebug() << "Cache::insert() - already cached; skipping " << name;
        return false;
    }
}

void Cache2::remove(QString name) {
    if(images.contains(name)) {
        images[name]->lock();
        auto *item = images.take(name);
        delete item;
        qDebug()<< "unlocked & removed " << name;
    }
}

void Cache2::clear() {
    for(auto name : images.keys()) {
        images[name]->lock();
        auto item = images.take(name);
        delete item;
    }
}

Image* Cache2::get(QString name) {
    if(images.contains(name)) {
        CacheItem *item = images[name];
        return item->getContents();
    } else {
        qDebug() << "Cache::get() - !!! returning NULL for " << name;
        return NULL;
    }
}

void Cache2::reserve(QString name) {
    if(images.contains(name)) {
        images[name]->lock();
        qDebug()<< "locked " << name;
    }
}

void Cache2::release(QString name) {
    if(images.contains(name)) {
        images[name]->unlock();
        qDebug()<< "unlocked " << name;
    }
}

// removes all items except the ones in list
void Cache2::trimTo(QStringList nameList) {
    for(auto name : images.keys()) {
        if(!nameList.contains(name)) {
            images[name]->lock();
            auto item = images.take(name);
            delete item;
        }
    }
}

const QList<QString> Cache2::keys() {
    return images.keys();
}
