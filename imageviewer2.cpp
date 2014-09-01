#include "imageviewer2.h"

ImageViewer2::ImageViewer2(): mCurrentScale(1.0)
{
    displaying = false;
    fitMode = FITORIGINAL;
}

void ImageViewer2::displayImage(Image *img) {
    image = img;
    if (image->getType() == GIF) {
        displaying = true;
        connect(image->getMovie(), SIGNAL(frameChanged(int)), this, SLOT(animate(int)));
        image->getMovie()->start();
        update();
    }
    if(image->getType() == STATIC) {
        displaying = true;
        disconnect(image->getMovie(), SIGNAL(frameChanged(int)), this, SLOT(animate(int)));
        currentPixmap = *(image->getImage());
        update();
    }
    fitDefault();
    recalculateGeometry();
}

void ImageViewer2::animate(int n)
{
    qDebug() << "animating: " << n;
    currentPixmap = image->getMovie()->currentPixmap();
    update();
}

void ImageViewer2::recalculateGeometry()
{
    mDrawingRect.setSize((currentPixmap.size()-mShrinkSize) * mCurrentScale);
    mDrawingRect.moveCenter(rect().center());
}

void ImageViewer2::paintEvent(QPaintEvent* event)
{
    if(displaying) {
        QPainter painter(this);
        painter.drawPixmap(mDrawingRect, currentPixmap);
    }
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

void ImageViewer2::slotFitHorizontal() {
    fitMode = FITHEIGHT;
    fitHorizontal();
    //recalculateGeometry();
    update();
}

void ImageViewer2::slotFitVertical() {
    fitMode = FITWIDTH;
    fitVertical();
    //recalculateGeometry();
    update();
}

void ImageViewer2::fitHorizontal()
{
    qDebug() << "Hi2";
    int hDifference = currentPixmap.width() - width();
    if (hDifference <= 0) // nothing to fit
    {
        mShrinkSize = QSize();
        return;
    }

    double aspect = (double) currentPixmap.height() / currentPixmap.width();
    mShrinkSize = QSize(hDifference, hDifference * aspect);
}

void ImageViewer2::fitVertical()
{
    qDebug() << "Hi1";
    int vDifference = currentPixmap.height() - height();
    if (vDifference <= 0) // nothing to fit
    {
        mShrinkSize = QSize();
        return;
    }

    double aspect = (double) currentPixmap.height() / currentPixmap.width();
    mShrinkSize = QSize(vDifference / aspect, vDifference);
}

void ImageViewer2::slotFitOriginal()
{
    fitMode = FITORIGINAL;
    mDrawingRect.setSize(currentPixmap.size());
    mShrinkSize=QSize();
    mDrawingRect.moveTopLeft(rect().center());
    //mDrawingRect.moveTopLeft();
    //recalculateGeometry();
    update();
}

void ImageViewer2::slotFitAll() {
    qDebug() << "Hi";
    fitMode = FITALL;
    int hDifference = height() - mDrawingRect.height();
    int vDifference = width() - mDrawingRect.width();

    if (hDifference > vDifference)
        fitHorizontal();
    else
        fitVertical();
    recalculateGeometry();
    update();
}

void ImageViewer2::fitDefault() {
    switch(fitMode) {
        case FITALL: slotFitAll(); break;
        case FITHEIGHT: slotFitHorizontal(); break;
        case FITWIDTH: slotFitVertical(); break;
        case FITORIGINAL: slotFitOriginal(); break;
    }
}

void ImageViewer2::resizeEvent(QResizeEvent* event)
{
    resize(event->size());
    
    /* TODO: set appropriate behaviour: 
    *     
    *     FitDefault
    *     FitHorizontal
    *     FitVertical
    *     FitOriginal
    *     
    *     default: FitDefault()
    */
//     if (!isZoomed)
    if(fitMode==FITALL || fitMode == FITWIDTH || fitMode==FITHEIGHT)
        fitDefault();
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
