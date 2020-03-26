#include "thumbnail.h"

Thumbnail::Thumbnail(QString _name, QString _label, int _size, std::shared_ptr<QPixmap> _pixmap)
    : mName(_name),
      mLabel(_label),
      mPixmap(_pixmap),
      mSize(_size)
{
    if(_pixmap)
        mHasAlphaChannel = _pixmap->hasAlphaChannel();
}

QString Thumbnail::name() {
    return mName;
}

QString Thumbnail::label() {
    return mLabel;
}

int Thumbnail::size() {
    return mSize;
}

bool Thumbnail::hasAlphaChannel() {
    return mHasAlphaChannel;
}

std::shared_ptr<QPixmap> Thumbnail::pixmap() {
    return mPixmap;
}
