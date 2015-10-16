#include "image.h"

void Image::safeDeleteSelf()
{
     mutex.lock(); // ensure we are not used
     mutex.unlock();
     delete this;
}

void Image::lock()
{
     mutex.lock();
}

void Image::unlock()
{
     mutex.unlock();
}

QString Image::getPath()
{
     return path;
}

bool Image::isLoaded()
{
     return loaded;
}

fileType Image::getType()
{
     return info->getType();
}

FileInfo *Image::getInfo()
{
     return info;
}

void Image::attachInfo ( FileInfo *_info )
{
     info = _info;
}
