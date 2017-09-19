#include "imageviewer.h"

// TODO: split into ImageViewerPrivate

ImageViewer::ImageViewer(QWidget *parent) : QWidget(parent),
    image(NULL),
    animation(NULL),
    isDisplaying(false),
    mouseWrapping(false),
    checkboardGridEnabled(false),
    expandImage(false),
    currentScale(1.0),
    fitWindowScale(0.125),
    minScale(0.125),
    maxScale(maxScaleLimit),
    scaleStep(0.10),
    imageFitMode(FIT_ORIGINAL)
{
    initOverlays();
    logo = new QPixmap();
    logo->load(":/res/images/logo.png");
    this->setMouseTracking(true);
    animationTimer = new QTimer(this);
    animationTimer->setSingleShot(true);
    cursorTimer = new QTimer(this);
    readSettings();
    connect(settings, SIGNAL(settingsChanged()),
            this, SLOT(readSettings()));
    connect(cursorTimer, SIGNAL(timeout()),
            this, SLOT(hideCursor()),
            Qt::UniqueConnection);
    desktopSize = QApplication::desktop()->size();
}

ImageViewer::~ImageViewer() {
}

void ImageViewer::initOverlays() {
    mapOverlay = new MapOverlay(this);
    mapOverlay->setEnabled(false);
    connect(mapOverlay, &MapOverlay::positionChanged, [ = ](float x, float y) {
        drawingRect.moveTo(x, y);
        centerImage();
        update();
        updateMap();
    });
}

void ImageViewer::startAnimation() {
    if(animation) {
        stopAnimation();
        connect(animationTimer, SIGNAL(timeout()), this, SLOT(nextFrame()));
        startAnimationTimer();
    }
}

void ImageViewer::stopAnimation() {
    if(animation) {
        animationTimer->stop();
        disconnect(animationTimer, SIGNAL(timeout()), this, SLOT(nextFrame()));
        animation->jumpToFrame(0);
    }
}
void ImageViewer::nextFrame() {
    if(animation) {
        if(!animation->jumpToNextFrame()) {
            animation->jumpToFrame(0);
        }
        QPixmap *newFrame = new QPixmap();
        *newFrame = animation->currentPixmap();
        startAnimationTimer();
        updateFrame(newFrame);
    }
}

void ImageViewer::startAnimationTimer() {
    if(animationTimer && animation) {
        animationTimer->start(animation->nextFrameDelay());
    }
}

void ImageViewer::displayAnimation(QMovie *_animation) {
    if(_animation && _animation->isValid()) {
        reset();
        animation = _animation;
        animation->jumpToFrame(0);
        readjust(animation->currentPixmap().size(),
                 animation->currentPixmap().rect());
        image = new QPixmap();
        *image = animation->currentPixmap().transformed(transform, Qt::SmoothTransformation);
        if(settings->transparencyGrid())
            drawTransparencyGrid();
        update();
        startAnimation();
    }
}

// display & initialize
void ImageViewer::displayImage(QPixmap *_image) {
    reset();
    if(_image) {
        image = _image;
        readjust(image->size(), image->rect());
        if(settings->transparencyGrid())
            drawTransparencyGrid();
        update();
        requestScaling();
    }
}

// reset state, remove image & stop animation
void ImageViewer::reset() {
    isDisplaying = false;
    if(animation) {
        stopAnimation();
        delete animation;
        animation = NULL;
    }
    if(image) {
        delete image;
        image = NULL;
    }
    mapOverlay->setEnabled(false);
}

// unsetImage, then update and show cursor
void ImageViewer::closeImage() {
    reset();
    update();
    showCursor();
}

void ImageViewer::adjustOverlays() {
    mapOverlay->updatePosition();
    updateMap();
}

// apply new image dimensions, fit mode, and readjust overlays
void ImageViewer::readjust(QSize _sourceSize, QRect _drawingRect) {
    isDisplaying = true;
    mapOverlay->setEnabled(true);
    sourceSize  = _sourceSize;
    drawingRect =  _drawingRect;
    updateMinScale();
    updateMaxScale();
    currentScale = 1.0;
    if(imageFitMode == FIT_FREE)
        imageFitMode = FIT_ALL;
    applyFitMode();
    adjustOverlays();
}

// takes scaled image
void ImageViewer::updateFrame(QPixmap *newFrame) {
    if(!animation && newFrame->size() != drawingRect.size()) {
        delete newFrame;
        return;
    }
    delete image;
    image = newFrame;
    if(checkboardGridEnabled)
        drawTransparencyGrid();
    update();
}

void ImageViewer::scrollUp() {
    scroll(0, -300);
}

void ImageViewer::scrollDown() {
    scroll(0, 300);
}

void ImageViewer::readSettings() {
    expandImage = settings->expandImage();
    maxResolutionLimit = (float)settings->maxZoomedResolution();
    maxScaleLimit = (float)settings->maximumZoom();
    updateMinScale();
    updateMaxScale();
    setFitMode(settings->imageFitMode());
    mouseWrapping = settings->mouseWrapping();
    checkboardGridEnabled = settings->transparencyGrid();
}

void ImageViewer::setExpandImage(bool mode) {
    expandImage = mode;
    updateMinScale();
    applyFitMode();
}

// scale at which current image fills the window
void ImageViewer::updateFitWindowScale() {
    float newMinScaleX = (float) width() / sourceSize.width();
    float newMinScaleY = (float) height() / sourceSize.height();
    if(newMinScaleX < newMinScaleY) {
        fitWindowScale = newMinScaleX;
    } else {
        fitWindowScale = newMinScaleY;
    }
}

bool ImageViewer::sourceImageFits() {
    return sourceSize.width() < width() && sourceSize.height() < height();
}

// limit min scale to window size
void ImageViewer::updateMinScale() {
    if(isDisplaying) {
        updateFitWindowScale();
        if(sourceImageFits()) {
            minScale = 1;
        } else {
            minScale = fitWindowScale;
        }
    }
}

// limit maxScale to MAX_SCALE_LIMIT
// then further limit to not exceed MAX_RESOLUTION
// so we dont go full retard on memory consumption
void ImageViewer::updateMaxScale() {
    maxScale = maxScaleLimit;
    float srcRes = (float)sourceSize.width() *
                          sourceSize.height() / 1000000;
    float maxRes = (float)maxScale * maxScale *
                          sourceSize.width() *
                          sourceSize.height() / 1000000;
    if(maxRes > maxResolutionLimit) {
        maxScale = (float)(sqrt(maxResolutionLimit) / sqrt(srcRes));
    }
}

// Scales drawingRect.
// drawingRect.topLeft() remains unchanged
void ImageViewer::setScale(float scale) {
    if(scale > maxScale) {
        currentScale = maxScale;
    } else if(scale <= minScale + FLT_EPSILON) {
        currentScale = minScale;
        if(imageFitMode == FIT_FREE && currentScale == fitWindowScale) {
            imageFitMode = FIT_ALL;
        }
    } else {
        currentScale = scale;
    }
    float w = scale * sourceSize.width();
    float h = scale * sourceSize.height();
    drawingRect.setWidth(w);
    drawingRect.setHeight(h);
    mapOverlay->updateMap(drawingRect);
}

// ##################################################
// ###################  RESIZE  #####################
// ##################################################

void ImageViewer::requestScaling() {
    if(!isDisplaying)
        return;
    if(image->size() != drawingRect.size() && !animation) {
        emit scalingRequested(drawingRect.size());
    }
}

void ImageViewer::drawTransparencyGrid() {
    if(image && image->hasAlphaChannel()) {
        QPainter painter(image);
        painter.setCompositionMode(QPainter::CompositionMode_DestinationOver);
        QColor dark(90,90,90,255);
        QColor light(140,140,140,255);
        int xCount, yCount;
        xCount = image->width() / CHECKBOARD_GRID_SIZE;
        yCount = image->height() / CHECKBOARD_GRID_SIZE;
        QRect square(0, 0, CHECKBOARD_GRID_SIZE, CHECKBOARD_GRID_SIZE);
        bool evenOdd;
        for(int i = 0; i <= yCount; i++) {
            evenOdd = (i % 2);
            for(int j = 0; j <= xCount; j++) {
                if(j % 2 == evenOdd)
                    painter.fillRect(square, light);
                square.translate(CHECKBOARD_GRID_SIZE, 0);
            }
            square.translate(0, CHECKBOARD_GRID_SIZE);
            square.moveLeft(0);
        }
        painter.fillRect(image->rect(), dark);
    }
}

// ##################################################
// ####################  PAINT  #####################
// ##################################################
void ImageViewer::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event)
    QPainter painter(this);
    //if(animation)
    //    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    if(image) {
        painter.drawPixmap(drawingRect, *image, image->rect());
        //zoomPoint for testing
        /*QPen pen(Qt::red);
        pen.setWidth(4);
        painter.setPen(pen);
        painter.drawPoint(zoomPoint);
        */
    } else {
        QRect logoRect(0,0,logo->width(), logo->height());
        logoRect.moveCenter(rect().center());
        painter.drawPixmap(logoRect, *logo, logo->rect());
    }
}

void ImageViewer::mousePressEvent(QMouseEvent *event) {
    QWidget::mousePressEvent(event);
    if(!isDisplaying)
        return;
    mapOverlay->enableVisibility(true);
    cursorTimer->stop();
    setCursor(QCursor(Qt::ArrowCursor));
    mouseMoveStartPos = event->pos();
    if(event->button() == Qt::LeftButton) {
        this->setCursor(QCursor(Qt::ClosedHandCursor));
    }
    if(event->button() == Qt::RightButton) {
        this->setCursor(QCursor(Qt::SizeVerCursor));
        setZoomPoint(event->pos());
    }
}

void ImageViewer::mouseMoveEvent(QMouseEvent *event) {
    QWidget::mouseMoveEvent(event);
    if(!isDisplaying)
        return;
    if(event->buttons() & Qt::LeftButton) {
        mouseWrapping?mouseDragWrapping(event):mouseDrag(event);
    } else if(event->buttons() & Qt::RightButton) {
        mouseZoom(event);
    } else {
        showCursor();
        cursorTimer->start(2000);
    }
}

void ImageViewer::mouseReleaseEvent(QMouseEvent *event) {
    QWidget::mouseReleaseEvent(event);
    if(!isDisplaying) {
        return;
    }
    this->setCursor(QCursor(Qt::ArrowCursor));
    if(event->button() == Qt::RightButton && imageFitMode != FIT_ALL) {
        //requestScaling();
        //fitDefault();
        //updateMap();
        //update();
    }
    mapOverlay->enableVisibility(false);
    updateMap();
}

// Okular-like cursor drag behavior
// TODO: fix multiscreen
void ImageViewer::mouseDragWrapping(QMouseEvent *event) {
    if( drawingRect.size().width() > this->width() ||
        drawingRect.size().height() > this->height() )
    {
        bool wrapped = false;
        QPoint newPos = mapToGlobal(event->pos()); //global
        QPoint delta = mouseMoveStartPos - event->pos(); // relative
        if(delta.x() && abs(delta.x()) < desktopSize.width() / 2) {
            int left = drawingRect.x() - delta.x();
            int right = left + drawingRect.width();
            if(left <= 0 && right > width()) {
                // wrap mouse along the X axis
                if(left+1 <= 0 && right-1 > width()) {
                    if(newPos.x() >= desktopSize.width() - 1) {
                        newPos.setX(2);
                        cursor().setPos(newPos);
                        wrapped = true;
                    } else if(newPos.x() <= 0) {
                        newPos.setX(desktopSize.width() - 2);
                        cursor().setPos(newPos);
                        wrapped = true;
                    }
                }
                // move image
                drawingRect.moveLeft(left);
            }
        }
        if(delta.y() && abs(delta.y()) < desktopSize.height() / 2) {
            int top = drawingRect.y() - delta.y();
            int bottom = top + drawingRect.height();
            if(top <= 0 && bottom > height()) {
                // wrap mouse along the Y axis
                if(top+1 <= 0 && bottom-1 > height()) {
                    if(newPos.y() >= desktopSize.height() - 1) {
                        newPos.setY(2);
                        cursor().setPos(newPos);
                        wrapped = true;
                    } else if(newPos.y() <= 0) {
                        newPos.setY(desktopSize.height() - 2);
                        cursor().setPos(newPos);
                        wrapped = true;
                    }
                }
                // move image
                drawingRect.moveTop(top);
            }
        }
        if(wrapped)
            mouseMoveStartPos = mapFromGlobal(newPos);
        else
            mouseMoveStartPos = event->pos();
        updateMap();
        update();
    }
}

// default drag behavior
void ImageViewer::mouseDrag(QMouseEvent *event) {
    if(drawingRect.size().width() > this->width() ||
            drawingRect.size().height() > this->height()) {
        mouseMoveStartPos -= event->pos();
        scroll(mouseMoveStartPos.x(), mouseMoveStartPos.y());
        mouseMoveStartPos = event->pos();
    }
}

void ImageViewer::mouseZoom(QMouseEvent *event) {
    float step = 0.005;
    int currentPos = event->pos().y();
    int moveDistance = mouseMoveStartPos.y() - currentPos;
    float newScale = currentScale + step * moveDistance;
    mouseMoveStartPos = event->pos();
    if(moveDistance < 0 && currentScale <= minScale) {
        return;
    } else if(moveDistance > 0 && newScale > maxScale) { // already at max zoom
        newScale = maxScale;
    } else if(moveDistance < 0 && newScale < minScale - FLT_EPSILON) { // at min zoom
        if(sourceImageFits() && expandImage) {
            fitNormal();
        } else {
            newScale = minScale;
            setFitAll();
        }
    } else {
        imageFitMode = FIT_FREE;
        scaleAroundZoomPoint(newScale);
        requestScaling();
    }
    update();
}

void ImageViewer::fitWidth() {
    if(isDisplaying) {
        float scale = (float) width() / sourceSize.width();
        if(!expandImage && scale > 1.0) {
            fitNormal();
        } else {
            setScale(scale);
            centerImage();
            if(drawingRect.height() > height())
                drawingRect.moveTop(0);
            update();
        }
    } else {
        centerImage();
    }
}

void ImageViewer::fitAll() {
    if(isDisplaying) {
        bool h = sourceSize.height() <= height();
        bool w = sourceSize.width() <= width();
        // source image fits entirely
        if(h && w && !expandImage) {
            fitNormal();
            return;
        } else { // doesnt fit
            setScale(expandImage?fitWindowScale:minScale);
            centerImage();
            update();
        }
    } else {
        centerImage();
    }
}

void ImageViewer::fitNormal() {
    if(!isDisplaying) {
        return;
    }
    setScale(1.0);
    centerImage();
    if(drawingRect.height() > height())
        drawingRect.moveTop(0);
    update();
}

void ImageViewer::setFitMode(ImageFitMode newMode) {
    imageFitMode = newMode;
    applyFitMode();
    updateMap();
    requestScaling();
}

void ImageViewer::applyFitMode() {
    switch(imageFitMode) {
        case FIT_ORIGINAL:
            fitNormal();
            break;
        case FIT_WIDTH:
            fitWidth();
            break;
        case FIT_ALL:
            fitAll();
            break;
        default: /* FREE etc */
            break;
    }
}

void ImageViewer::updateMap() {
    mapOverlay->updateMap(drawingRect);
}

void ImageViewer::setFitOriginal() {
    setFitMode(FIT_ORIGINAL);
}

void ImageViewer::setFitWidth() {
    setFitMode(FIT_WIDTH);
}

void ImageViewer::setFitAll() {
    setFitMode(FIT_ALL);
}

void ImageViewer::resizeEvent(QResizeEvent *event) {
    Q_UNUSED(event)
    updateMinScale();
    if(imageFitMode == FIT_FREE || imageFitMode == FIT_ORIGINAL) {
        centerImage();
    } else {
        applyFitMode();
    }
    mapOverlay->updatePosition();
    updateMap();
    update();
    requestScaling();
}

// center image if it is smaller than parent
// align image's corner to window corner if needed
void ImageViewer::centerImage() {
    if(drawingRect.height() <= height()) {
        drawingRect.moveTop((height() - drawingRect.height()) / 2);
    } else {
        snapEdgeVertical();
    }
    if(drawingRect.width() <= width()) {
        drawingRect.moveLeft((width() - drawingRect.width()) / 2);
    } else {
        snapEdgeHorizontal();
    }
}

void ImageViewer::snapEdgeHorizontal() {
    if(drawingRect.x() > 0 && drawingRect.right() > width()) {
        drawingRect.moveLeft(0);
    }
    if(width() - drawingRect.x() > drawingRect.width()) {
        drawingRect.moveRight(width());
    }
}

void ImageViewer::snapEdgeVertical() {
    if(drawingRect.y() > 0 && drawingRect.bottom() > height()) {
        drawingRect.moveTop(0);
    }
    if(height() - drawingRect.y() > drawingRect.height()) {
        drawingRect.moveBottom(height());
    }
}

// scroll viewport and do update()
void ImageViewer::scroll(int dx, int dy) {
    if(drawingRect.size().width() > this->width()) {
        scrollX(dx);
    }
    if(drawingRect.size().height() > this->height()) {
        scrollY(dy);
    }
    updateMap();
    update();
}

// scroll viewport
void ImageViewer::scrollX(int dx) {
    if(dx) {
        int left = drawingRect.x() - dx;
        int right = left + drawingRect.width();
        if(left > 0)
            left = 0;
        else if (right <= width())
            left = width() - drawingRect.width();
        if(left <= 0) {
            drawingRect.moveLeft(left);
        }
    }
}

// scroll viewport
void ImageViewer::scrollY(int dy) {
    if(dy) {
        int top = drawingRect.y() - dy;
        int bottom = top + drawingRect.height();
        if(top > 0)
            top = 0;
        else if (bottom <= height())
            top = height() - drawingRect.height();
        if(top <= 0) {
            drawingRect.moveTop(top);
        }
    }
}

void ImageViewer::setZoomPoint(QPoint pos) {
    zoomPoint = pos;
    zoomDrawRectPoint.setX((float) (zoomPoint.x() - drawingRect.x())
                                / drawingRect.width());
    zoomDrawRectPoint.setY((float) (zoomPoint.y() - drawingRect.y())
                                / drawingRect.height());
}

// scale image around zoom point,
// so that point's position relative to window remains unchanged
void ImageViewer::scaleAroundZoomPoint(float newScale) {
    setScale(newScale);
    float mappedX = drawingRect.width() * zoomDrawRectPoint.x() + drawingRect.left();
    float mappedY = drawingRect.height() * zoomDrawRectPoint.y() + drawingRect.top();
    int diffX = mappedX - zoomPoint.x();
    int diffY = mappedY - zoomPoint.y();
    drawingRect.moveLeft(drawingRect.left() - diffX);
    drawingRect.moveTop(drawingRect.top() - diffY);
    centerImage();
}

// zoom in around viewport center
void ImageViewer::zoomIn() {
    setZoomPoint(rect().center());
    doZoomIn();
}

// zoom out around viewport center
void ImageViewer::zoomOut() {
    setZoomPoint(rect().center());
    doZoomOut();
}

// zoom in around cursor
void ImageViewer::zoomInCursor() {
    if(underMouse()) {
        setZoomPoint(mapFromGlobal(cursor().pos()));
        doZoomIn();
    } else {
        zoomIn();
    }
}

// zoom out around cursor
void ImageViewer::zoomOutCursor() {
    if(underMouse()) {
        setZoomPoint(mapFromGlobal(cursor().pos()));
        doZoomOut();
    } else {
        zoomOut();
    }
}

void ImageViewer::doZoomIn() {
    if(!isDisplaying) {
        return;
    }
    float newScale = currentScale + scaleStep;
    if(newScale == currentScale) //skip if minScale
        return;
    if(newScale > maxScale)
        newScale = maxScale;
    imageFitMode = FIT_FREE;
    scaleAroundZoomPoint(newScale);
    updateMap();
    update();
    requestScaling();
}

void ImageViewer::doZoomOut() {
    if(!isDisplaying) {
        return;
    }
    float newScale = currentScale - scaleStep;
    if(newScale == currentScale) //skip if maxScale
        return;
    if(newScale < minScale - FLT_EPSILON)
        newScale = minScale;
    imageFitMode = FIT_FREE;
    scaleAroundZoomPoint(newScale);
    updateMap();
    update();
    requestScaling();
}

ImageFitMode ImageViewer::fitMode() {
    return imageFitMode;
}

void ImageViewer::hideCursor() {
    cursorTimer->stop();
    if(this->underMouse()) {
        setCursor(QCursor(Qt::BlankCursor));
    }
}

void ImageViewer::showCursor() {
    cursorTimer->stop();
    setCursor(QCursor(Qt::ArrowCursor));
}
