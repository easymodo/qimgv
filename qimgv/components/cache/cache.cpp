#include "cache.h"

Cache::Cache() {
}

bool Cache::contains(QString path) const {
    return items.contains(path);
}

bool Cache::insert(std::shared_ptr<Image> img) {
    if(img) {
        if(items.contains(img->filePath())) {
            return false;
        } else {
            items.insert(img->filePath(), new CacheItem(img));
            return true;
        }
    }
    // TODO: what state returns here ??
    return true;
}

void Cache::remove(QString path) {
    if(items.contains(path)) {
        items[path]->lock();
        auto *item = items.take(path);
        delete item;
    }
}

void Cache::clear() {
    for(auto path : items.keys()) {
        items[path]->lock();
        auto item = items.take(path);
        delete item;
    }
}

std::shared_ptr<Image> Cache::get(QString path) {
    if(items.contains(path)) {
        CacheItem *item = items.value(path);
        return item->getContents();
    }
    return nullptr;
}

bool Cache::reserve(QString path) {
    if(items.contains(path)) {
        items[path]->lock();
        return true;
    }
    return false;
}

bool Cache::release(QString path) {
    if(items.contains(path)) {
        items[path]->unlock();
        return true;
    }
    return false;
}

/* Removes all items except the ones in list.
 * Images carrying unsaved edits are kept regardless: dropping one would make
 * the next load read the untouched file back from disk, silently throwing the
 * user's work away.
 */
void Cache::trimTo(QStringList pathList) {
    for(auto path : items.keys()) {
        if(pathList.contains(path))
            continue;
        auto contents = items[path]->getContents();
        if(contents && contents->isEdited())
            continue;
        items[path]->lock();
        auto *item = items.take(path);
        delete item;
    }
}

const QList<QString> Cache::keys() const {
    return items.keys();
}
