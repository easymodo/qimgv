#include "imageviewer.h"

// TODO: split into ImageViewerPrivate

ImageViewer::ImageViewer(QWidget *parent) : QWidget(parent),
    pixmap(nullptr),
    movie(nullptr),
    mIsDisplaying(false),
    mouseWrapping(false),
    checkboardGridEnabled(false),
    expandImage(false),
    smoothAnimatedImages(true),
    mouseInteraction(MOUSE_NONE),
    mCurrentScale(1.0),
    fitWindowScale(0.125),
    minScale(0.125),
    maxScale(maxScaleLimit),
    imageFitMode(FIT_ORIGINAL)
{
    setMouseTracking(true);
    posAnimation = new QPropertyAnimation(this, "drawPos");
    posAnimation->setEasingCurve(QEasingCurve::OutCubic);
    posAnimation->setDuration(animationSpeed);
    animationTimer = new QTimer(this);
    animationTimer->setSingleShot(true);
    readSettings();
    connect(settings, SIGNAL(settingsChanged()),
            this, SLOT(readSettings()));
    desktopSize = QApplication::desktop()->size();
}

ImageViewer::~ImageViewer() {
}

void ImageViewer::startAnimation() {
    if(movie) {
        stopAnimation();
        connect(animationTimer, SIGNAL(timeout()), this, SLOT(nextFrame()));
        startAnimationTimer();
    }
}

void ImageViewer::stopAnimation() {
    if(movie) {
        animationTimer->stop();
        disconnect(animationTimer, SIGNAL(timeout()), this, SLOT(nextFrame()));
        movie->jumpToFrame(0);
    }
}
void ImageViewer::nextFrame() {
    if(movie) {
        if(!movie->jumpToNextFrame()) {
            movie->jumpToFrame(0);
        }
        std::unique_ptr<QPixmap> newFrame(new QPixmap());
        *newFrame = movie->currentPixmap();
        if(movie->frameCount() > 1) {
            startAnimationTimer();
        }
        replacePixmap(std::move(newFrame));
    }
}

void ImageViewer::startAnimationTimer() {
    if(animationTimer && movie) {
        animationTimer->start(movie->nextFrameDelay());
    }
}

void ImageViewer::displayAnimation(std::unique_ptr<QMovie> _movie) {
    if(_movie && _movie->isValid()) {
        reset();
        movie = std::move(_movie);
        movie->jumpToFrame(0);
        readjust(movie->currentPixmap().size(),
                 movie->currentPixmap().rect());
        pixmap = std::unique_ptr<QPixmap>(new QPixmap());
        *pixmap = movie->currentPixmap().transformed(transform, Qt::SmoothTransformation);
        if(settings->transparencyGrid())
            drawTransparencyGrid();
        //update();
        startAnimation();
    }
}

// display & initialize
void ImageViewer::displayImage(std::unique_ptr<QPixmap> _pixmap) {
    reset();
    if(_pixmap) {
        pixmap = std::move(_pixmap);
        readjust(pixmap->size(), pixmap->rect());
        if(settings->transparencyGrid())
            drawTransparencyGrid();
        update();
        requestScaling();
    }
}

// reset state, remove image & stop animation
void ImageViewer::reset() {
    mIsDisplaying = false;
    stopPosAnimation();
    pixmap.reset(nullptr);
    stopAnimation();
    movie.reset(nullptr);
}

// unsetImage, then update and show cursor
void ImageViewer::closeImage() {
    reset();
    update();
}

// apply new image dimensions and fit mode
void ImageViewer::readjust(QSize _sourceSize, QRect _drawingRect) {
    mIsDisplaying = true;
    mSourceSize  = _sourceSize;
    drawingRect =  _drawingRect;
    updateMinScale();
    updateMaxScale();
    setScale(1.0f);
    if(imageFitMode == FIT_FREE)
        imageFitMode = FIT_WINDOW;
    applyFitMode();
}

// new pixmap must be the size of drawingRect
void ImageViewer::replacePixmap(std::unique_ptr<QPixmap> newFrame) {
    if(!movie && newFrame->size() != drawingRect.size()) {
        return;
    }
    pixmap = std::move(newFrame);
    if(checkboardGridEnabled)
        drawTransparencyGrid();
    update();
}

bool ImageViewer::isDisplaying() {
    return mIsDisplaying;
}

void ImageViewer::scrollUp() {
    scroll(0, -SCROLL_DISTANCE, true);
}

void ImageViewer::scrollDown() {
    scroll(0, SCROLL_DISTANCE, true);
}

void ImageViewer::scrollLeft() {
    scroll(-SCROLL_DISTANCE, 0, true);
}

void ImageViewer::scrollRight() {
    scroll(SCROLL_DISTANCE, 0, true);
}

void ImageViewer::readSettings() {
    smoothAnimatedImages = settings->smoothAnimatedImages();
    expandImage = settings->expandImage();
    maxResolutionLimit = static_cast<float>(settings->maxZoomedResolution());
    maxScaleLimit = static_cast<float>(settings->maximumZoom());
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

void ImageViewer::show() {
    setMouseTracking(false);
    QWidget::show();
    setMouseTracking(true);
}

void ImageViewer::hide() {
    setMouseTracking(false);
    QWidget::hide();
}

// scale at which current image fills the window
void ImageViewer::updateFitWindowScale() {
    float newMinScaleX = (float) width()  * devicePixelRatioF() / mSourceSize.width();
    float newMinScaleY = (float) height() * devicePixelRatioF() / mSourceSize.height();
    if(newMinScaleX < newMinScaleY) {
        fitWindowScale = newMinScaleX;
    } else {
        fitWindowScale = newMinScaleY;
    }
}

bool ImageViewer::sourceImageFits() {
    return mSourceSize.width() < width() * devicePixelRatioF() && mSourceSize.height() < height() * devicePixelRatioF();
}

void ImageViewer::propertySetDrawPos(QPoint newPos) {
    if(newPos != drawingRect.topLeft()) {
        drawingRect.moveTopLeft(newPos);
        update();
    }
}

QPoint ImageViewer::propertyDrawPos() {
    return drawingRect.topLeft();
}

// limit min scale to window size
void ImageViewer::updateMinScale() {
    if(mIsDisplaying) {
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
    float srcRes = (float)mSourceSize.width() *
                          mSourceSize.height() / 1000000;
    float maxRes = (float)maxScale * maxScale *
                          mSourceSize.width() *
                          mSourceSize.height() / 1000000;
    if(maxRes > maxResolutionLimit) {
        maxScale = (float)(sqrt(maxResolutionLimit) / sqrt(srcRes));
    }
}

// Scales drawingRect.
// drawingRect.topLeft() remains unchanged
void ImageViewer::setScale(float scale) {
    if(scale > maxScale) {
        mCurrentScale = maxScale;
    } else if(scale <= minScale + FLT_EPSILON) {
        mCurrentScale = minScale;
        if(imageFitMode == FIT_FREE && mCurrentScale == fitWindowScale) {
            imageFitMode = FIT_WINDOW;
        }
    } else {
        mCurrentScale = scale;
    }
    float w = scale * mSourceSize.width();
    float h = scale * mSourceSize.height();
    drawingRect.setWidth(w);
    drawingRect.setHeight(h);
    emit scaleChanged(mCurrentScale);
}

// ##################################################
// ###################  RESIZE  #####################
// ##################################################

void ImageViewer::requestScaling() {
    if(!mIsDisplaying)
        return;
    if(pixmap->size() != drawingRect.size() && !movie) {
        emit scalingRequested(drawingRect.size());
    }
}

void ImageViewer::drawTransparencyGrid() {
    if(pixmap && pixmap->hasAlphaChannel()) {
        QPainter painter(pixmap.get());
        painter.setCompositionMode(QPainter::CompositionMode_DestinationOver);
        QColor dark(90,90,90,255);
        QColor light(140,140,140,255);
        int xCount, yCount;
        xCount = pixmap->width() / CHECKBOARD_GRID_SIZE;
        yCount = pixmap->height() / CHECKBOARD_GRID_SIZE;
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
        painter.fillRect(pixmap->rect(), dark);
    }
}

// ##################################################
// ####################  PAINT  #####################
// ##################################################
void ImageViewer::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event)
    QPainter painter(this);
    if(movie && smoothAnimatedImages)
        painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    if(pixmap) {
        pixmap->setDevicePixelRatio(devicePixelRatioF());
        // maybe pre-calculate size so we wont do it in every draw?
        QRectF dpiAdjusted(drawingRect.topLeft() / devicePixelRatioF(),
                           drawingRect.size() / devicePixelRatioF());
        //qDebug() << dpiAdjusted;
        painter.drawPixmap(dpiAdjusted, *pixmap, pixmap->rect());
    }
}

//  Right button zooming / dragging logic
//  mouseMoveStartPos: stores the previous mouseMoveEvent() position,
//                     used to calculate delta.
//  mousePressPos: used to filter out accidental zoom events
//  mouseInteraction: tracks which action we are performing since the last mousePressEvent()
//
void ImageViewer::mousePressEvent(QMouseEvent *event) {
    QWidget::mousePressEvent(event);
    if(!mIsDisplaying)
        return;
    mouseMoveStartPos = event->pos();
    mousePressPos = mouseMoveStartPos;
    if(event->button() == Qt::RightButton) {
        setZoomPoint(event->pos() * devicePixelRatioF());
    }
}

void ImageViewer::mouseMoveEvent(QMouseEvent *event) {
    QWidget::mouseMoveEvent(event);
    if(!mIsDisplaying)
        return;
    if(event->buttons() & Qt::LeftButton && mouseInteraction != MOUSE_ZOOM) {
        if(cursor().shape() != Qt::ClosedHandCursor) {
            setCursor(Qt::ClosedHandCursor);
        }
        mouseInteraction = MOUSE_DRAG;
        mouseWrapping ? mouseDragWrapping(event) : mouseDrag(event);
    } else if(event->buttons() & Qt::RightButton && mouseInteraction != MOUSE_DRAG) {
        // filter out possible mouse jitter by ignoring low delta drags
        if(mouseInteraction == MOUSE_ZOOM || abs(mousePressPos.y() - event->pos().y()) > ZOOM_THRESHOLD) {
            if(cursor().shape() != Qt::SizeVerCursor) {
                setCursor(Qt::SizeVerCursor);
            }
            mouseInteraction = MOUSE_ZOOM;
            mouseMoveZoom(event);
        }
    }
}

void ImageViewer::mouseReleaseEvent(QMouseEvent *event) {
    unsetCursor();
    QWidget::mouseReleaseEvent(event);
    if(!mIsDisplaying) {
        mouseInteraction = MOUSE_NONE;
        if(event->button() == Qt::RightButton) {
            emit rightClicked();
        } else if(event->button() == Qt::LeftButton) {
            emit leftClicked();
        }
        return;
    }
    if(mouseInteraction == MOUSE_NONE) {
        if(event->button() == Qt::RightButton) {
            emit rightClicked();
        } else if(event->button() == Qt::LeftButton) {
            emit leftClicked();
        }
    }
    mouseInteraction = MOUSE_NONE;
}

//  Okular-like cursor drag behavior.
//  TODO: looks like it's broken when you have multiple screens with
//        different resolution (height in particular).
//
//  ...it is probably a bad idea to fiddle with this
inline
void ImageViewer::mouseDragWrapping(QMouseEvent *event) {
    if( drawingRect.size().width()  > width()  * devicePixelRatioF() ||
        drawingRect.size().height() > height() * devicePixelRatioF() )
    {
        bool wrapped = false;
        QPoint newPos = mapToGlobal(event->pos()); //global
        QPoint delta = mouseMoveStartPos - event->pos(); // relative
        if(delta.x() && abs(delta.x()) < desktopSize.width() / 2) {
            int left = drawingRect.x() - delta.x();
            int right = left + drawingRect.width();
            if(left <= 0 && right > width() * devicePixelRatioF()) {
                // wrap mouse along the X axis
                if(left+1 <= 0 && right-1 > width() * devicePixelRatioF()) {
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
            if(top <= 0 && bottom > height() * devicePixelRatioF()) {
                // wrap mouse along the Y axis
                if(top+1 <= 0 && bottom-1 > height() * devicePixelRatioF()) {
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
        update();
    }
}

//  simple drag behavior
inline
void ImageViewer::mouseDrag(QMouseEvent *event) {
    if( drawingRect.size().width()  > width()  * devicePixelRatioF() ||
        drawingRect.size().height() > height() * devicePixelRatioF() )
    {
        mouseMoveStartPos -= event->pos();
        scroll(mouseMoveStartPos.x() * devicePixelRatioF(),
               mouseMoveStartPos.y() * devicePixelRatioF(),
               false);
        mouseMoveStartPos = event->pos();
    }
}

//  zooming while the right button is pressed
//  note: on reaching min zoom level the fitMode is set to FIT_WINDOW;
//        mid-zoom it is set to FIT_FREE.
//        FIT_FREE mode does not persist when changing images.
inline
void ImageViewer::mouseMoveZoom(QMouseEvent *event) {
    float stepMultiplier = 0.003f; // this one feels ok
    int currentPos = event->pos().y();
    int moveDistance = mouseMoveStartPos.y() - currentPos;
    float newScale = mCurrentScale * (1.0f + stepMultiplier * moveDistance);
    mouseMoveStartPos = event->pos();
    if(moveDistance < 0 && mCurrentScale <= minScale) {
        return;
    } else if(moveDistance > 0 && newScale > maxScale) { // already at max zoom
        newScale = maxScale;
    } else if(moveDistance < 0 && newScale < minScale - FLT_EPSILON) { // at min zoom
        if(sourceImageFits() && expandImage) {
            setFitOriginal();
        } else {
            newScale = minScale;
            setFitWindow();
        }
    } else {
        imageFitMode = FIT_FREE;
        scaleAroundZoomPoint(newScale);
        requestScaling();
    }
    update();
}

void ImageViewer::fitWidth() {
    if(mIsDisplaying) {
        float scale = (float)width() * devicePixelRatioF() / mSourceSize.width();
        if(!expandImage && scale > 1.0f) {
            fitNormal();
        } else {
            setScale(scale);
            centerImage();
            if(drawingRect.height() > height()*devicePixelRatioF())
                drawingRect.moveTop(0);
            update();
        }
    } else {
        centerImage();
    }
}

void ImageViewer::fitWindow() {
    if(mIsDisplaying) {
        bool h = mSourceSize.height() <= height() * devicePixelRatioF();
        bool w = mSourceSize.width()  <= width()  * devicePixelRatioF();
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
    if(!mIsDisplaying) {
        return;
    }
    setScale(1.0);
    centerImage();
    if(drawingRect.height() > height() * devicePixelRatioF()) {
        drawingRect.moveTop(0);
    }
    update();
}

void ImageViewer::setFitMode(ImageFitMode newMode) {
    stopPosAnimation();
    imageFitMode = newMode;
    applyFitMode();
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
        case FIT_WINDOW:
            fitWindow();
            break;
        default: /* FREE etc */
            break;
    }
}

void ImageViewer::setFitOriginal() {
    setFitMode(FIT_ORIGINAL);
}

void ImageViewer::setFitWidth() {
    setFitMode(FIT_WIDTH);
}

void ImageViewer::setFitWindow() {
    setFitMode(FIT_WINDOW);
}

void ImageViewer::resizeEvent(QResizeEvent *event) {
    Q_UNUSED(event)
    // Qt emits some unnecessary resizeEvents on startup
    // so we try to ignore them
    if(this->isVisible()) {
        stopPosAnimation();
        updateMinScale();
        if(imageFitMode == FIT_FREE || imageFitMode == FIT_ORIGINAL) {
            centerImage();
        } else {
            applyFitMode();
        }
        update();
        requestScaling();
    }
}

// center image if it is smaller than parent
// align image's corner to window corner if needed
void ImageViewer::centerImage() {
    if(drawingRect.height() <= height() * devicePixelRatioF()) {
        drawingRect.moveTop((height() * devicePixelRatioF() - drawingRect.height()) / 2);
    } else {
        snapEdgeVertical();
    }
    if(drawingRect.width() <= width() * devicePixelRatioF()) {
        drawingRect.moveLeft((width() * devicePixelRatioF() - drawingRect.width()) / 2);
    } else {
        snapEdgeHorizontal();
    }
}

void ImageViewer::snapEdgeHorizontal() {
    if(drawingRect.x() > 0 && drawingRect.right() > width() * devicePixelRatioF()) {
        drawingRect.moveLeft(0);
    }
    if(width() * devicePixelRatioF() - drawingRect.x() > drawingRect.width()) {
        drawingRect.moveRight(width() * devicePixelRatioF());
    }
}

void ImageViewer::snapEdgeVertical() {
    if(drawingRect.y() > 0 && drawingRect.bottom() > height() * devicePixelRatioF()) {
        drawingRect.moveTop(0);
    }
    if(height()*devicePixelRatioF() - drawingRect.y() > drawingRect.height()) {
        drawingRect.moveBottom(height() * devicePixelRatioF());
    }
}

void ImageViewer::stopPosAnimation() {
    if(posAnimation->state() == QAbstractAnimation::Running)
        posAnimation->stop();
}

// scroll viewport and do update()
inline
void ImageViewer::scroll(int dx, int dy, bool smooth) {
    stopPosAnimation();
    QPoint destTopLeft = drawingRect.topLeft();
    if(drawingRect.size().width() > width() * devicePixelRatioF()) {
        destTopLeft.setX(scrolledX(dx));
    }
    if(drawingRect.size().height() > height() * devicePixelRatioF()) {
        destTopLeft.setY(scrolledY(dy));
    }
    if(smooth) {
        posAnimation->setStartValue(drawingRect.topLeft());
        posAnimation->setEndValue(destTopLeft);
        posAnimation->start(QAbstractAnimation::KeepWhenStopped);
    } else {
        drawingRect.moveTopLeft(destTopLeft);
        update();
    }
}

inline
int ImageViewer::scrolledX(int dx) {
    int newXPos = drawingRect.left();
    if(dx) {
        int left = drawingRect.x() - dx;
        int right = left + drawingRect.width();
        if(left > 0)
            left = 0;
        else if (right <= width() * devicePixelRatioF())
            left = width() * devicePixelRatioF() - drawingRect.width();
        if(left <= 0) {
            newXPos = left;
        }
    }
    return newXPos;
}

inline
int ImageViewer::scrolledY(int dy) {
    int newYPos = drawingRect.top();
    if(dy) {
        int top = drawingRect.y() - dy;
        int bottom = top + drawingRect.height();
        if(top > 0)
            top = 0;
        else if (bottom <= height() * devicePixelRatioF())
            top = height() * devicePixelRatioF() - drawingRect.height();
        if(top <= 0) {
            newYPos = top;
        }
    }
    return newYPos;
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
    setZoomPoint(rect().center() * devicePixelRatioF());
    doZoomIn();
}

// zoom out around viewport center
void ImageViewer::zoomOut() {
    setZoomPoint(rect().center() * devicePixelRatioF());
    doZoomOut();
}

// zoom in around cursor
void ImageViewer::zoomInCursor() {
    if(underMouse()) {
        setZoomPoint(mapFromGlobal(cursor().pos()) * devicePixelRatioF());
        doZoomIn();
    } else {
        zoomIn();
    }
}

// zoom out around cursor
void ImageViewer::zoomOutCursor() {
    if(underMouse()) {
        setZoomPoint(mapFromGlobal(cursor().pos()) * devicePixelRatioF());
        doZoomOut();
    } else {
        zoomOut();
    }
}

void ImageViewer::doZoomIn() {
    if(!mIsDisplaying) {
        return;
    }
    float newScale = mCurrentScale * 1.1f;
    if(newScale == mCurrentScale) //skip if minScale
        return;
    if(newScale > maxScale)
        newScale = maxScale;
    imageFitMode = FIT_FREE;
    scaleAroundZoomPoint(newScale);
    update();
    requestScaling();
}

void ImageViewer::doZoomOut() {
    if(!mIsDisplaying) {
        return;
    }
    float newScale = mCurrentScale * 0.9f;
    if(newScale == mCurrentScale) //skip if maxScale
        return;
    if(newScale < minScale - FLT_EPSILON)
        newScale = minScale;
    imageFitMode = FIT_FREE;
    scaleAroundZoomPoint(newScale);
    update();
    requestScaling();
}

ImageFitMode ImageViewer::fitMode() {
    return imageFitMode;
}

QRect ImageViewer::imageRect() {
    return drawingRect;
}

float ImageViewer::currentScale() {
    return mCurrentScale;
}

QSize ImageViewer::sourceSize() {
    return mSourceSize;
}
