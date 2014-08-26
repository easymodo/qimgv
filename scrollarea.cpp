#include "scrollarea.h"

ScrollArea::ScrollArea(QWidget *parent) :
    QScrollArea(parent),
    hBar(this->horizontalScrollBar()),
    vBar(this->verticalScrollBar()),
    label(new CustomLabel())
{
    this->setMouseTracking(true);
    this->setAlignment(Qt::AlignCenter);
    this->setPalette(QPalette(Qt::black));
    this->setFrameShape(QFrame::NoFrame);
    this->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);

    hBar->setStyleSheet("QScrollBar {height:0px;}");
    vBar->setStyleSheet("QScrollBar {height:0px;}");
    
    setWidget(label);
}

void ScrollArea::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        this->setCursor(QCursor(Qt::ClosedHandCursor));
        lastDragPosition = event->pos();
    }
}

void ScrollArea::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        this->setCursor(QCursor(Qt::ArrowCursor));
        lastDragPosition = event->pos();
    }
}

/* enables mouse dragging
 */
void ScrollArea::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        QPoint difference = lastDragPosition - event->pos();

        int hMaxDistance = hBar->maximum();
        int vMaxDistance = vBar->maximum();
        
        int stepX = difference.x() + hBar->value();
        int stepY = difference.y() + vBar->value();
        
        this->hBar->setValue(stepX > hMaxDistance ? hMaxDistance : stepX);
        vBar->setValue(stepY > vMaxDistance ? vMaxDistance : stepY);

        lastDragPosition = event->pos();
//         emit scrollbarChanged();
    }
}

void ScrollArea::mouseDoubleClickEvent(QMouseEvent *event) {
    emit sendDoubleClick();
}

void ScrollArea::wheelEvent(QWheelEvent *event) {
    event->ignore();
}

void ScrollArea::resizeEvent(QResizeEvent *event) {
    QScrollArea::resizeEvent(event);
    mSize = event->size();
    mAspect = mSize.height() / mSize.width();
    emit resized();
}

void ScrollArea::keyPressEvent(QKeyEvent *event) {
    event->ignore();
}

void ScrollArea::displayImage(Image *img)
{

    if(img->getType() == NONE) {
        return;
    }
    else if(img->getType() == STATIC) {
        label->setPixmap(img->getPixmap());
        currentImage = img;
    }
    else if(img->getType() == GIF) {

        currentImage->getMovie()->stop();
        label->setMovie(img->getMovie());
        currentImage = img;
    }
    img->getPath();
    fitImageDefault();
}

void ScrollArea::fitImageHorizontal()
{                                                  
    QSize imageSize = label->size();
    int difference = imageSize.width() - width();
    double aspectRatio = imageSize.height()/imageSize.width();
    doudle aspectRatio =
    
    if (difference <= 0)
        return;
    
    imageSize.rwidth() -= difference;
    imageSize.rheight() -= difference * label->aspectRatio();
    
    imageViewer->resize(imageSize);
}

void ScrollArea::fitImageVertical()
{                                                  
    QSize imageSz = imageViewer->size();
    int difference = imageSz.height() - height();
    
    if (difference <= 0)
        return;
    
    imageSz.rwidth() -= difference / imageViewer->getAspect();
    imageSz.rheight() -= difference;
    
    imageViewer->resize(imageSz);
}

void ScrollArea::fitImageDefault() {
    QSize imageSz = imageViewer->size();
    
    int diffHorizontal = width() - imageSz.width();
    int diffVertical = height() - imageSz.height();
    
    if (diffHorizontal < diffVertical) {
        fitImageHorizontal();
    } else  {
        fitImageVertical();
    }
}

void ScrollArea::fitImageOriginal()
{
    imageViewer->fitOriginalSize();
}

void ScrollArea::scaleImage(double factor)
{
    QSize size = imageViewer->size() * factor;
    imageViewer->resize(size);
    
//     hBar->setValue(hBar->value() * factor);
//     vBar->setValue(vBar->value() * factor);
}
