#include "imageviewer.h"

ImageViewer::ImageViewer(): QWidget(),
    currentScale(1.0),
    imageFitMode(NORMAL),
    currentImage(NULL),
    isDisplayingFlag(false),
    errorFlag(false)
{
    initMap();
    image.load(":/images/res/logo.png");
    drawingRect = image.rect();
    fitDefault();
}

ImageViewer::ImageViewer(QWidget* parent): QWidget(parent),
    currentScale(1.0),
    imageFitMode(NORMAL),
    currentImage(NULL),
    isDisplayingFlag(false),
    errorFlag(false)
{
    initMap();
    image.load(":/images/res/logo.png");
    drawingRect = image.rect();
}

ImageViewer::~ImageViewer() {
    delete currentImage;
}

void ImageViewer::initMap() {
    mapOverlay = new MapOverlay(this);
}

bool ImageViewer::scaled() const {
    return scale() != 1.0;
}

void ImageViewer::stopAnimation() {
    if(currentImage->getType()==GIF) {
        currentImage->getMovie()->stop();
        disconnect(currentImage->getMovie(), SIGNAL(frameChanged(int)),
                   this, SLOT(onAnimation()));
    }
}

void ImageViewer::startAnimation() {
    connect(currentImage->getMovie(), SIGNAL(frameChanged(int)),
            this, SLOT(onAnimation()),Qt::DirectConnection);
    currentImage->getMovie()->start();
}


void ImageViewer::freeImage() {
    if (currentImage!=NULL) {
        stopAnimation();
        currentImage->setInUse(false);
    }
}
void ImageViewer::displayImage(Image* i) {
    currentImage = i;
    if(i->getType()==NONE) {
        //empty or corrupted image
        image.load(":/images/res/error.png");
        isDisplayingFlag = false;
        errorFlag=true;
    }
    else {
        errorFlag=false;
        isDisplayingFlag = true;
        if(currentImage->getType() == STATIC) {
            image = *currentImage->getImage();
        }
        else if (currentImage->getType() == GIF) {
            currentImage->getMovie()->jumpToFrame(0);
            image = currentImage->getMovie()->currentImage();
            maxScale = defaultMaxScale;
            startAnimation();
        }
        calculateMaxScale();
    }
    drawingRect = image.rect();
    currentScale = 1.0;
    if(imageFitMode == FREE)
        imageFitMode = NORMAL;
    fitDefault();
    emit imageChanged();
}

void ImageViewer::calculateMaxScale() {
    float newMaxScaleX = (float)width()/image.width();
    float newMaxScaleY = (float)height()/image.height();
    if(newMaxScaleX < newMaxScaleY) {
        maxScale = newMaxScaleX;
    }
    else {
        maxScale = newMaxScaleY;
    }
    if(maxScale > defaultMaxScale)
        maxScale = defaultMaxScale;
}

float ImageViewer::scale() const {
    return currentScale;
}

void ImageViewer::setScale(float scale) {
    if(currentScale!=scale) {
        if (scale >= minScale) {
            currentScale = minScale;
        }
        else if(scale <= maxScale) {
            currentScale = maxScale;
            //resizePolicy = ALL;
        }
        else {
            currentScale = scale;
        }
        QSize sz = image.size();
        sz = sz.scaled(sz * scale, Qt::KeepAspectRatio);
        drawingRect.setSize(sz);
    }
}

void ImageViewer::onAnimation() {
    image = currentImage->getMovie()->currentImage();
    update();
}

// ##############################################
void ImageViewer::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.fillRect(rect(), QBrush(QColor(0, 0, 0)));
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    //int time = clock();
    painter.drawImage(drawingRect, image);
    //qDebug() << "VIEWER: draw time: " << clock() - time;
}
// ##############################################

void ImageViewer::mousePressEvent(QMouseEvent* event) {
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
            update();
            updateMap();
        }
    }
    if (event->buttons() & Qt::RightButton && isDisplayingFlag) {
        float step = (maxScale - minScale) / -300.0;
        int currentPos = event->pos().y();
        int moveDistance = mouseMoveStartPos.y() - currentPos;
        float newScale = currentScale + step*(moveDistance);
        if(moveDistance > 0 && newScale > minScale) {
                newScale = minScale;
        }
        if(moveDistance < 0 && newScale < maxScale-FLT_EPSILON) {
                newScale = maxScale;
        }
        if(newScale == currentScale) {
            return;
        }
        imageFitMode = FREE;
        scaleAround(fixedZoomPoint, newScale);
        mouseMoveStartPos = event->pos();
    }
}

void ImageViewer::mouseReleaseEvent(QMouseEvent* event) {
    mouseMoveStartPos = event->pos();
    fixedZoomPoint = event->pos();
    this->setCursor(QCursor(Qt::ArrowCursor));
}

void ImageViewer::fitWidth() {
    if(isDisplayingFlag) {
        float scale = (float) width() / image.width();
        setScale(scale);
        imageAlign();
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
    if(isDisplayingFlag) {
        bool h = image.height() <= this->height();
        bool w = image.width() <= this->width();
        if(h && w) {
            fitNormal();
        }
        else {
            float widgetAspect = (float) height() / width();
            float drawingAspect = (float) drawingRect.height() /
                                          drawingRect.width();
            if(widgetAspect > drawingAspect) {
                float scale = (float) width() / image.width();
                setScale(scale);
            }
            else {
                float scale = (float) height() / image.height();
                setScale(scale);
            }
            if(scaled()) {
                drawingRect.moveCenter(rect().center());
                update();
            }
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
   setScale(1.0);
   centerImage();
   update();
}

void ImageViewer::fitDefault() {
    switch(imageFitMode) {
        case NORMAL: fitNormal(); break;
        case WIDTH: fitWidth(); break;
        case ALL: fitAll(); break;
        default: fitNormal(); break;
    }
    updateMap();
}

void ImageViewer::updateMap() {
    mapOverlay->updateMap(size(), drawingRect);
}

void ImageViewer::slotFitNormal() {
    imageFitMode = NORMAL;
    fitDefault();
}

void ImageViewer::slotFitWidth() {
    imageFitMode = WIDTH;
    fitDefault();
}

void ImageViewer::slotFitAll() {
    imageFitMode = ALL;
    fitDefault();
}

void ImageViewer::resizeEvent(QResizeEvent* event) {
    calculateMaxScale();
    if(imageFitMode == FREE || imageFitMode == NORMAL) {
        imageAlign();
    }
    else {
        fitDefault();
    }
    mapOverlay->updateMap(size(),drawingRect);
    mapOverlay->updatePosition();
}

void ImageViewer::mouseDoubleClickEvent(QMouseEvent *event) {
    if(event->button() == Qt::RightButton) {
        emit sendRightDoubleClick();
        //slotFitNormal();
    }
    else {
        emit sendDoubleClick();
    }
}

// center image
void ImageViewer::centerImage() {
    drawingRect.moveCenter(rect().center());
    imageAlign();
}

// fix image positions
void ImageViewer::imageAlign() {
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

void ImageViewer::scaleAround(QPointF p, float newScale) {
    float xPos = (float)(p.x()-drawingRect.x())/drawingRect.width();
    float oldPx = (float)xPos*drawingRect.width();
    float oldX = drawingRect.x();
    float yPos = (float)(p.y()-drawingRect.y())/drawingRect.height();
    float oldPy = (float)yPos*drawingRect.height();
    float oldY = drawingRect.y();
    setScale(newScale);
    float newPx = (float)xPos*drawingRect.width();
    drawingRect.moveLeft(oldX - (newPx-oldPx));
    float newPy = (float)yPos*drawingRect.height();
    drawingRect.moveTop(oldY - (newPy-oldPy));
    imageAlign();
    update();
    updateMap();
}

void ImageViewer::slotZoomIn() {
    if(isDisplayingFlag) {
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
    }
}

void ImageViewer::slotZoomOut() {
    if(isDisplayingFlag) {
        float newScale = scale() - scaleStep;
        if(newScale == currentScale) //skip if maxScale
            return;
        if(newScale < maxScale-FLT_EPSILON) {
            newScale = maxScale;
        }
        imageFitMode = FREE;
        fixedZoomPoint = rect().center();
        scaleAround(fixedZoomPoint, newScale);
    }
}

Image* ImageViewer::getCurrentImage() const {
    return currentImage;
}

bool ImageViewer::isDisplaying() {
    return isDisplayingFlag;
}
