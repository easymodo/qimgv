#include "imageviewer.h"

ImageViewer::ImageViewer(QWidget* parent): QWidget(parent),
    source(NULL),
    isDisplayingFlag(false),
    errorFlag(false),
    currentScale(1.0),
    maxScale(1.0),
    minScale(4.0),
    scaleStep(0.05),
    imageFitMode(NORMAL)
{
    initOverlays();
    bgColor.setRgb(17,17,17,255);
    image = new QImage();
    image->load(":/images/res/logo.png");
    drawingRect = image->rect();
    this->setMouseTracking(true);
    resizeTimer = new QTimer(this);
    resizeTimer->setSingleShot(true);
    cursorTimer = new QTimer(this);
    connect(resizeTimer, SIGNAL(timeout()),
            this, SLOT(resizeImage()),
            Qt::UniqueConnection);
    connect(cursorTimer, SIGNAL(timeout()),
            this, SLOT(hideCursor()),
            Qt::UniqueConnection);
}

ImageViewer::~ImageViewer() {
    delete source;
}

void ImageViewer::initOverlays() {
    mapOverlay = new MapOverlay(this);
    mapOverlay->setEnabled(false);
    connect(mapOverlay, &MapOverlay::positionChanged, [=](float x, float y)
    {
        drawingRect.moveTo(x, y);
        alignImage();
        update();
    });

    cropOverlay = new CropOverlay(this);
    connect(cropOverlay, SIGNAL(cropSelected(QRect)),
            this, SIGNAL(cropSelected(QRect)));
}

bool ImageViewer::imageIsScaled() const {
    return scale() != 1.0;
}

void ImageViewer::stopAnimation() {
    if(source->getType()==GIF) {
        source->getMovie()->stop();
        disconnect(source->getMovie(), SIGNAL(frameChanged(int)),
                   this, SLOT(onAnimation()));
    }
}

void ImageViewer::startAnimation() {
    resizeTimer->stop();
    disconnect(resizeTimer, SIGNAL(timeout()), this, SLOT(resizeImage()));
    connect(source->getMovie(), SIGNAL(frameChanged(int)),
            this, SLOT(onAnimation()),Qt::DirectConnection);
    source->getMovie()->start();
}

void ImageViewer::onAnimation() {
    *image = source->getMovie()->currentImage();
    update();
}

void ImageViewer::freeImage() {
    if (source!=NULL) {
        stopAnimation();
    }
}

void ImageViewer::displayImage(Image* i) {
    resizeTimer->stop();
    source = i;
    if(i->getType()==NONE) {
        //empty or corrupted image
        image->load(":/images/res/error.png");
        isDisplayingFlag = false;
        mapOverlay->setEnabled(false);
        errorFlag=true;
    }
    else {
        errorFlag=false;
        isDisplayingFlag = true;
        if(source->getType() == STATIC) {
           *image = *source->getImage();
            connect(resizeTimer, SIGNAL(timeout()),
                    this, SLOT(resizeImage()), Qt::UniqueConnection);
        }
        else if (source->getType() == GIF) {
            source->getMovie()->jumpToFrame(0);
            *image = source->getMovie()->currentImage();
            startAnimation();
        }
        updateMaxScale();
        updateMinScale();
        cursorTimer->start(2000);
    }
    drawingRect = image->rect();
    currentScale = 1.0;
    if(imageFitMode == FREE)
        imageFitMode = ALL;
    fitDefault();
    mapOverlay->setEnabled(true);
    updateMap();
    cropOverlay->setRealSize(source->size());
    cropOverlay->setImageArea(drawingRect, currentScale);
    cropOverlay->hide();
    mapOverlay->updatePosition();
    update();
    resizeTimer->start(0);
    //emit imageChanged();
}

void ImageViewer::redisplay() {
    displayImage(source);
}

void ImageViewer::crop() {
    if(cropOverlay->isHidden() && isDisplaying()) {
        cursorTimer->stop();
        cropOverlay->display();
    } else {
        cropOverlay->hide();
    }
}

void ImageViewer::updateMaxScale() {
    if(isDisplaying()) {
        if (source->width() < width() &&
            source->height() < height()) {
            maxScale = 1;
            return;
        }
        float newMaxScaleX = (float)width()/source->width();
        float newMaxScaleY = (float)height()/source->height();
        if(newMaxScaleX < newMaxScaleY) {
            maxScale = newMaxScaleX;
        }
        else {
            maxScale = newMaxScaleY;
        }
        //qDebug() << "SET maxScale=" << maxScale;
    }
}

void ImageViewer::updateMinScale() {
    minScale=3.0;
    float imgSize = source->width()*source->height()/1000000;
    float maxSize =
            minScale*source->width()*source->height()/1000000;
    if(maxSize>25) {
        minScale=sqrt(25/imgSize);
    }
    //qDebug() << "SET minScale = " << minScale;
}

float ImageViewer::scale() const {
    return currentScale;
}

void ImageViewer::setScale(float scale) {
        if (scale > minScale) {
            currentScale = minScale;
        }
        else if(scale <= maxScale+FLT_EPSILON) {
            currentScale = maxScale;
            if(imageFitMode == FREE)
                imageFitMode = ALL; // TODO: update gui checkbox
        }
        else {
            currentScale = scale;
        }
        float h = scale*source->height();
        float w = scale*source->width();
        drawingRect.setHeight(h);
        drawingRect.setWidth(w);

        mapOverlay->updateMap(drawingRect);
        cropOverlay->setImageArea(drawingRect, currentScale);
}

// ##################################################
// ###################  RESIZE  #####################
// ##################################################

void ImageViewer::resizeImage() {
    if(!isDisplaying()) {
        return;
    }
    resizeTimer->stop();
    if(image->size() != drawingRect.size()) {
        //int time = clock();
        delete image;
        float sourceSize = source->width()*source->height()/1000000;
        float size = drawingRect.width()*drawingRect.height()/1000000;
        if(currentScale==1.0) {
            image = new QImage();
            *image = *source->getImage();
        } else if(currentScale<1.0) { // downscale
            if(sourceSize>15) {
                if(size>10) {
                    // large src, larde dest
                    image = fastScale(source->getImage(), drawingRect.size(), false);
                } else if(size>4 && size<10){
                    // large src, medium dest
                    image = fastScale(source->getImage(), drawingRect.size(), true);
                } else {
                    // large src, low dest
                    image = bilinearScale(source->getImage(), drawingRect.size());
                }
            } else {
                // low src
                image = bilinearScale(source->getImage(), drawingRect.size());
            }
        } else {
            if(sourceSize>10) { // upscale
                // large src
                image = fastScale(source->getImage(), drawingRect.size(), false);
            } else {
                // low src
                image = fastScale(source->getImage(), drawingRect.size(), true);
            }
        }
/*
        qDebug() << "###### scaling ######";
        qDebug() << currentScale;
        qDebug() << "size: " << size;
        qDebug() << "srcSize: " << sourceSize;
        qDebug() << "time: " << clock()-time;
*/
        update();
    }
}

// ##################################################
// ####################  PAINT  #####################
// ##################################################
void ImageViewer::paintEvent(QPaintEvent* event) {
//     qDebug() << "paint at " << clock();
    Q_UNUSED( event )
    QPainter painter(this);
    painter.fillRect(rect(), QBrush(bgColor));
    if(source && source->getType() == GIF) {
        painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    }
    //int time = clock();
    painter.drawImage(drawingRect, *image, image->rect());
    //qDebug() << "VIEWER: draw time: " << clock() - time;
}

void ImageViewer::mousePressEvent(QMouseEvent* event) {
    if(!isDisplaying()) {
        return;
    }
    cursorTimer->stop();
    setCursor(QCursor(Qt::ArrowCursor));
    mouseMoveStartPos = event->pos();
    if (event->button() == Qt::LeftButton) {
        this->setCursor(QCursor(Qt::ClosedHandCursor));
    }
    if (event->button() == Qt::RightButton) {
        this->setCursor(QCursor(Qt::SizeVerCursor));
        fixedZoomPoint = event->pos();
    }
}

void ImageViewer::mouseMoveEvent(QMouseEvent* event) {
    if(!isDisplaying()) {
        return;
    }
    cursorTimer->stop();
    if (event->buttons() & Qt::LeftButton) {
        if(drawingRect.size().width() > this->width() ||
           drawingRect.size().height() > this->height())
        {
            mouseMoveStartPos -= event->pos();
            int left = drawingRect.x() - mouseMoveStartPos.x();
            int top = drawingRect.y() - mouseMoveStartPos.y();
            int right = left + drawingRect.width();
            int bottom = top + drawingRect.height();
            if (left <= 0 && right > size().width())
                drawingRect.moveLeft(left);
            if (top <= 0 && bottom > size().height())
                drawingRect.moveTop(top);
            mouseMoveStartPos = event->pos();
            updateMap();
            update();
        }
    } else if (event->buttons() & Qt::RightButton) {
        float step = (maxScale - minScale) / -500.0;
        int currentPos = event->pos().y();
        int moveDistance = mouseMoveStartPos.y() - currentPos;
        float newScale = currentScale + step*(moveDistance);
        mouseMoveStartPos = event->pos();

        if(moveDistance < 0 && currentScale <= maxScale) {
            return;
        } else if(moveDistance > 0 && newScale > minScale) { // already at max zoom
                newScale = minScale;
        } else if(moveDistance < 0 && newScale < maxScale-FLT_EPSILON) { // a min zoom
                newScale = maxScale;
                slotFitAll();
        } else {
            imageFitMode = FREE;
            scaleAround(fixedZoomPoint, newScale);
            resizeTimer->stop();
            resizeTimer->start(75);
        }
        update();
    } else {
        setCursor(QCursor(Qt::ArrowCursor));
        cursorTimer->start(2000);
    }
}

void ImageViewer::mouseReleaseEvent(QMouseEvent* event) {
    if(!isDisplaying()) {
        return;
    }
    mouseMoveStartPos = event->pos();
    fixedZoomPoint = event->pos();
    this->setCursor(QCursor(Qt::ArrowCursor));
    if(event->button() == Qt::RightButton && imageFitMode!=ALL) {
        //resizeTimer->start(0);
        //fitDefault();
        //updateMap();
        //update();
    }
    cursorTimer->start(2000);
}

void ImageViewer::fitWidth() {
    if(isDisplaying()) {
        float scale = (float) width() / source->size().width();
        setScale(scale);
        centerImage();
        if(drawingRect.height()>height())
            drawingRect.moveTop(0);
        update();
    }
    else if(errorFlag) {
        fitNormal();
    }
    else {
        centerImage();
    }
}

void ImageViewer::fitAll() {
    if(isDisplaying()) {
        bool h = source->height() <= height();
        bool w = source->width() <= width();
        // source image fits entirely
        if(h && w) {
            fitNormal();
            return;
        }
        else { // doesnt fit
            setScale(maxScale);
            centerImage();
            update();
        }
    }
    else if(errorFlag) {
        fitNormal();
    }
    else {
        centerImage();
    }
}

void ImageViewer::fitNormal() {
    if(!isDisplaying()) {
        return;
    }
   setScale(1.0);
   centerImage();
   update();
}

void ImageViewer::fitDefault() {
    switch(imageFitMode) {
        case NORMAL: fitNormal(); break;
        case WIDTH: fitWidth(); break;
        case ALL: fitAll(); break;
        default: /* FREE etc */ break;
    }
    cropOverlay->setImageArea(drawingRect, currentScale);
}

void ImageViewer::updateMap() {
    mapOverlay->updateMap(drawingRect);
}

void ImageViewer::slotFitNormal() {
    imageFitMode = NORMAL;
    fitDefault();
    updateMap();
    resizeTimer->start(0);
}

void ImageViewer::slotFitWidth() {
    imageFitMode = WIDTH;
    fitDefault();
    updateMap();
    resizeTimer->start(0);
}

void ImageViewer::slotFitAll() {
    imageFitMode = ALL;
    fitDefault();
    updateMap();
    resizeTimer->start(0);
}

void ImageViewer::resizeEvent(QResizeEvent* event) {
    Q_UNUSED( event )

    updateMaxScale();
    if(imageFitMode == FREE || imageFitMode == NORMAL) {
        alignImage();
    }
    else {
        fitDefault();
    }
    mapOverlay->updatePosition();
    cropOverlay->hide();
    updateMap();
    update();
    resizeTimer->start(150);
}

void ImageViewer::mouseDoubleClickEvent(QMouseEvent *event) {
    if(event->button() == Qt::RightButton) {
        emit sendRightDoubleClick();
    }
    else {
        emit sendDoubleClick();
    }
}

// centers image inside window rectangle
void ImageViewer::centerImage() {
    int x = drawingRect.x();
    drawingRect.moveCenter(rect().center());
}

// centers image inside window
// aligns image corner to window corner if needed
void ImageViewer::alignImage() {
    if(drawingRect.height() <= height()) {
            drawingRect.moveTop((height()-drawingRect.height())/2);
        }
        else {
            fixAlignVertical();
        }
        if(drawingRect.width() <= width()) {
            drawingRect.moveLeft((width()-drawingRect.width())/2);
        }
        else {
            fixAlignHorizontal();
        }
}

void ImageViewer::fixAlignHorizontal() {
    if(drawingRect.x()>0 && drawingRect.right()>width()) {
        drawingRect.moveLeft(0);
    }
    if(width()-drawingRect.x()>drawingRect.width()) {
        drawingRect.moveRight(width());
    }
}

void ImageViewer::fixAlignVertical() {
    if(drawingRect.y()>0 && drawingRect.bottom()>height()) {
        drawingRect.moveTop(0);
    }
    if(height()-drawingRect.y()>drawingRect.height()) {
        drawingRect.moveBottom(height());
    }
}

// scales image around point, so point's position
// relative to window remains unchanged
void ImageViewer::scaleAround(QPointF p, float newScale) {
    float oldX = drawingRect.x();
    float xPos = (float)(p.x()-oldX)/(drawingRect.width()-oldX);
    float oldPx = (float)xPos*drawingRect.width();
    float oldY = drawingRect.y();
    float yPos = (float)(p.y()-oldY)/drawingRect.height();
    float oldPy = (float)yPos*drawingRect.height();
    setScale(newScale);
    //qDebug() << "new xPos = " << xPos;
    float newPx = (float)xPos*drawingRect.width();
    drawingRect.moveLeft(oldX - (newPx-oldPx));
    float newPy = (float)yPos*drawingRect.height();
    drawingRect.moveTop(oldY - (newPy-oldPy));
    alignImage();
}

void ImageViewer::slotZoomIn() {
    if(!isDisplaying()) {
        return;
    }
    float newScale = scale() + scaleStep;
    if(newScale == currentScale) { //skip if minScale
        return;
    }
    if(newScale > minScale) {
        newScale = minScale;
    }
    imageFitMode = FREE;
    fixedZoomPoint = rect().center();
    scaleAround(fixedZoomPoint, newScale);
    updateMap();
    update();
    resizeTimer->start(100);
}

void ImageViewer::slotZoomOut() {
    if(!isDisplaying()) {
        return;
    }
    float newScale = scale() - scaleStep;
    if(newScale == currentScale) //skip if maxScale
        return;
    if(newScale < maxScale-FLT_EPSILON) {
        newScale = maxScale;
    }
    imageFitMode = FREE;
    fixedZoomPoint = rect().center();
    scaleAround(fixedZoomPoint, newScale);
    updateMap();
    update();
    resizeTimer->start(100);
}

Image* ImageViewer::getCurrentImage() const {
    return source;
}

bool ImageViewer::isDisplaying() const {
    return isDisplayingFlag;
}

void ImageViewer::hideCursor() {
    cursorTimer->stop();
    setCursor(QCursor(Qt::BlankCursor));
}
