#include "scrollarea.h"

ScrollArea::ScrollArea(QWidget *parent) : QScrollArea(parent), 
    imageViewer(new ImageViewer()), mHBar(horizontalScrollBar()),
    mVBar(verticalScrollBar())
{
    this->setMouseTracking(true);
    this->setAlignment(Qt::AlignCenter);
    this->setPalette(QPalette(Qt::black));
    this->setFrameShape(QFrame::NoFrame);
    this->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);

    mHBar->setStyleSheet("QScrollBar {height:0px;}");
    mVBar->setStyleSheet("QScrollBar {width:0px;}");
    
    setWidget(imageViewer);
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
//         imageViewer->set
        int hMaxDistance = mHBar->maximum();
        int vMaxDistance = mVBar->maximum();
        
        int stepX = difference.x() + mHBar->value();
        int stepY = difference.y() + mVBar->value();
        
        mHBar->setValue(stepX > hMaxDistance ? hMaxDistance : stepX);
        mVBar->setValue(stepY > vMaxDistance ? vMaxDistance : stepY);

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

int ScrollArea::getAspect() const
{
    return mAspect;
}

void ScrollArea::setImagePath(const QString& path)
{
    imageViewer->setImagePath(path);
    fitImageDefault();
}

void ScrollArea::fitImageHorizontal()
{                                                  
    QSize imageSz = imageViewer->size();
    int difference = imageSz.width() - width();
    
    if (difference <= 0)
        return;
    
    imageSz.rwidth() -= difference;
    imageSz.rheight() -= difference * imageViewer->getAspect();
    
    imageViewer->resize(imageSz);
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
