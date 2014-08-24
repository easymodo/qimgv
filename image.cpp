#include "image.h"
#include <qt/QtGui/qmovie.h>

Image::Image() : QObject(), mFormat(), mType(0), mPath()
{
}

Image::Image(QString path) : QObject(), mFormat(), mType(0), mPath(path)
{
    loadImage(path);
}

Image::~Image()
{
    free(mSource);
}

void Image::loadImage(QString path)
{
    QFile file(path);
    if (!file.exists())
    {
        mType = Type::NONE;
        qDebug() << "File " << path << " not exists";
        return;
    }
    
    if (!file.open(QIODevice::ReadOnly))
    {
        mType = Type::NONE;
        qDebug() << "Can't open file" << path;
        return;
    }
    
    if (file.read((char*)&mFormat, 2) == -1)
    {
        mType = Type::NONE;
        qDebug() << "Can't read file";
        return;
    }
    
    mType = mFormat == Format::GIF ? Type::DYNAMIC : Type::STATIC;
    
    if (mFormat == Type::DYNAMIC) {
        mSource = new QMovie(path);
    } else {
        mSource = new QPixmap(path);
    }
}

void* Image::getSource() const
{
    return mSource;
}

int Image::getFormat() const
{
    return mFormat;
}

int Image::getType() const
{
    return mType;
}

QString Image::getPath() const
{
    return mPath;
}
