#include "CustomLabel.h"

CustomLabel::CustomLabel(QWidget *parent) : QLabel(parent)
{
    this->setAlignment(Qt::AlignCenter);
    this->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    this->setScaledContents(true);
}

void CustomLabel::resizeEvent(QResizeEvent *event) {
    QLabel::resizeEvent(event);
    emit resized();
}

void CustomLabel::setPixmap(QPixmap *pixmap) {
    this->QLabel::setPixmap(*pixmap);
}


/*
bool CustomLabel::setImagePath(const QString& path)
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
*/
