#include "thumbnail.h"

Thumbnail::Thumbnail(QString _name, QString _label, int _size, std::shared_ptr<const QPixmap> _pixmap)
    : mName(_name),
      mLabel(_label),
      mSize(_size),
      mPixmap(_pixmap)
{

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

std::shared_ptr<const QPixmap> Thumbnail::pixmap() {
    return mPixmap;
}
