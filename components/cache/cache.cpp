#include "cache.h"

Cache::Cache() {
}

bool Cache::contains(QString name) {
    return items.contains(name);
}

bool Cache::insert(QString name, std::shared_ptr<Image> img) {
    if(items.contains(name)) {
        return false;
    } else {
        items.insert(name, new CacheItem(img));
        return true;
    }
}

void Cache::remove(QString name) {
    if(items.contains(name)) {
        items[name]->lock();
        auto *item = items.take(name);
        delete item;
    }
}

void Cache::clear() {
    for(auto name : items.keys()) {
        items[name]->lock();
        auto item = items.take(name);
        delete item;
    }
}

std::shared_ptr<Image> Cache::get(QString name) {
    if(items.contains(name)) {
        CacheItem *item = items.value(name);
        return item->getContents();
    } else {
        qDebug() << "Cache::get() - !!! returning nullptr for " << name << ". There is a bug in logic somewhere!";
        return nullptr;
    }
}

bool Cache::reserve(QString name) {
    if(items.contains(name)) {
        items[name]->lock();
        return true;
    } else {
        return false;
    }
}

bool Cache::release(QString name) {
    if(items.contains(name)) {
        items[name]->unlock();
        return true;
    } else {
        return false;
    }
}

// removes all items except the ones in list
void Cache::trimTo(QStringList nameList) {
    for(auto name : items.keys()) {
        if(!nameList.contains(name)) {
            //qDebug() << "CACHE-RM: locking.. " << name;
            items[name]->lock();
            //qDebug() << "CACHE-RM: LOCKED: " << name;
            auto *item = items.take(name);
            delete item;
        }
    }
}

const QList<QString> Cache::keys() {
    return items.keys();
}
