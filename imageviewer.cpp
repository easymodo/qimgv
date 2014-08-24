#include "imageviewer.h"

ImageViewer::ImageViewer(QWidget *parent) : QLabel(parent)
{
}

void ImageViewer::resizeEvent(QResizeEvent *event) {
    QLabel::resizeEvent(event);
    emit resized();
}

QString ImageViewer::getImagePath() const
{
    mImage->getPath();
}

bool ImageViewer::setImagePath(const QString& path)
{
    mImage = new Image(path);
    int type = mImage->getType();
    
    switch (type)
    {
        case Type::DYNAMIC:
        {
            QMovie* movie = (QMovie*) mImage->getSource(); 
            QLabel::setMovie(movie);
            movie->start();
            mSize = movie->scaledSize();
            break;
        }
        case Type::STATIC:
        {
            QPixmap* pixmap = (QPixmap*) mImage->getSource();
            QLabel::setPixmap(*pixmap);
            mSize = pixmap->size();
            break;
        }
        default:
        {
            return false;
        }
    }
    
    resize(mSize);
    mAspect = (double) mSize.height() / mSize.width();
}

double ImageViewer::getAspect() const
{
    return mAspect;
}

QString ImageViewer::getImageInfo() const
{
    
}

void ImageViewer::fitOriginalSize()
{
    resize(mSize);
}
