#include "imageviewer2.h"

ImageViewer2::ImageViewer2(): mCurrentScale(1.0)
{
}


void ImageViewer2::displayImage(Image *img)
{
    image = img;
    if (image->getType() == GIF) {
        connect(image->getMovie(), SIGNAL(frameChanged(int)), this, SLOT(update()));
    }
    if(image->getType() == STATIC) {
        disconnect(image->getMovie(), SIGNAL(frameChanged(int)), this, SLOT(update()));
        currentPixmap = *(image->getPixmap());
        update();
    }
}

void ImageViewer2::animate()
{
    currentPixmap = image->getMovie()->currentPixmap();
}
/*
void ImageViewer2::onAnimation()
{
    AnimationInfo animationInfo = mAnimations.next();
    mImage = animationInfo.image();
    
    recalculateGeometry();
    update();
    
    mAnimationTimer.start(animationInfo.delay());
}
*/
void ImageViewer2::recalculateGeometry()
{
    mDrawingRect.setSize((currentPixmap.size() - mShrinkSize) * mCurrentScale);
    mDrawingRect.moveCenter(rect().center());
}

void ImageViewer2::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.drawPixmap(mDrawingRect, currentPixmap);
}

void ImageViewer2::mousePressEvent(QMouseEvent* event)
{
    mCursorMovedDistance = event->pos();
}

void ImageViewer2::mouseMoveEvent(QMouseEvent* event)
{
    mCursorMovedDistance -= event->pos();
    
    int left = mDrawingRect.x() - mCursorMovedDistance.x();
    int top = mDrawingRect.y() - mCursorMovedDistance.y();
    int right = left + mDrawingRect.width();
    int bottom = top + mDrawingRect.height();
    
    if (left < 0 && right > size().width())
        mDrawingRect.moveLeft(left);
    
    if (top < 0 && bottom > size().height())
        mDrawingRect.moveTop(top);
    
    mCursorMovedDistance = event->pos();
    update();
}

void ImageViewer2::mouseReleaseEvent(QMouseEvent* event)
{
    mCursorMovedDistance = event->pos();
}

void ImageViewer2::increaseScale(double value)
{
    double possibleScale = mCurrentScale + value;
    bool zoomOut = mCurrentScale > possibleScale && mCurrentScale <= 1.0;
    if (imageInsideWidget(possibleScale) && zoomOut)
        return;
    
    mCurrentScale = possibleScale > maxScale ? maxScale : possibleScale;
    
    /*     
    *     TODO: scale image to cursor position because 
    *     current zoom reset point that cursor are looking
    */ 
    
    recalculateGeometry();
    update();
}

bool ImageViewer2::imageInsideWidget(double aspect)
{
    QSize imageSz = mDrawingRect.size() * aspect;
    return imageSz.height() < height() && imageSz.width() < width();
}

void ImageViewer2::fitImageHorizontal()
{
    int hDifference = currentPixmap.width() - width();
    if (hDifference <= 0) // nothing to fit
    {
        mShrinkSize = QSize();
        return;
    }

    double aspect = (double) currentPixmap.height() / currentPixmap.width();
    mShrinkSize = QSize(hDifference, hDifference * aspect);
    
    update();
}

void ImageViewer2::fitImageVertical()
{
    int vDifference = currentPixmap.height() - height();
    if (vDifference <= 0) // nothing to fit
    {
        mShrinkSize = QSize();
        return;
    }

    double aspect = (double) currentPixmap.height() / currentPixmap.width();
    mShrinkSize = QSize(vDifference / aspect, vDifference);

    update();
}

void ImageViewer2::fitImageDefault()
{
    int hDifference = height() - mDrawingRect.height();
    int vDifference = width() - mDrawingRect.width();
    
    if (hDifference > vDifference)
        fitImageHorizontal();
    else
        fitImageVertical();
}

void ImageViewer2::fitImageOriginal()
{
    mDrawingRect.setSize(currentPixmap.size());
}

void ImageViewer2::resizeEvent(QResizeEvent* event)
{
    resize(event->size());
    
    /* TODO: set appropriate behaviour: 
    *     
    *     fitImageDefault
    *     fitImageHorizontal
    *     fitImageVertical
    *     fitImageOriginal
    *     
    *     default: fitImageDefault()
    */
//     if (!isZoomed)
        fitImageDefault();
    recalculateGeometry();
}

void ImageViewer2::wheelEvent(QWheelEvent* event)
{
    double forward = event->angleDelta().y();
/*     
*  TODO: set non const divident {10000}
*/     
    
    increaseScale(forward / 1200);
//     isZoomed = true;
    event->accept();
}

// 
// This set any scale but skipping window intersection check
// 

void ImageViewer2::setScale(double scale)
{
    mCurrentScale = 0;
    increaseScale(scale);
}

/**************** Private classes ****************/


AnimationInfo::AnimationInfo(int delay, const QImage& image)
{
    mDelay = delay;
    mImage = image;
    nulled = false;
}

int AnimationInfo::delay()
{
    return mDelay;
}

const QImage& AnimationInfo::image() const
{
    return mImage;
}
bool AnimationInfo::isNull() const
{
    return nulled;
}

AnimationList::AnimationList(): current(0) {}

inline const AnimationInfo& AnimationList::next()
{
    if (current >= size()) current = 0;
    return at(current++);
}

void AnimationList::reset()
{
    current = 0;
}
