#include "imageviewer.h"

ImageViewer::ImageViewer(QWidget *parent) : QWidget(parent),
    isDisplayingFlag(false),
    errorFlag(false),
    mouseWrapping(false),
    transparencyGridEnabled(false),
    currentScale(1.0),
    maxScale(2.0),
    minScale(4.0),
    scaleStep(0.16),
    imageFitMode(NORMAL) {
    initOverlays();
    image = new QPixmap();
    image->load(":/images/res/logo_dev.png");
    drawingRect = image->rect();
    this->setMouseTracking(true);
    resizeTimer = new QTimer(this);
    resizeTimer->setSingleShot(true);
    cursorTimer = new QTimer(this);
    readSettings();
    connect(settings, SIGNAL(settingsChanged()),
            this, SLOT(readSettings()));
    connect(resizeTimer, SIGNAL(timeout()),
            this, SLOT(resizeImage()),
            Qt::UniqueConnection);
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
        alignImage();
        update();
        updateMap();
    });

    cropOverlay = new CropOverlay(this);
}

bool ImageViewer::imageIsScaled() const {
    return scale() != 1.0;
}

// display & initialize
void ImageViewer::displayImage(QPixmap *_image) {
    delete image;
    resizeTimer->stop();
    sourceSize  = _image->size();
    drawingRect =  _image->rect();

    errorFlag = false;
    isDisplayingFlag = true;
    image = _image;

    mapOverlay->setEnabled(true);


    updateMaxScale();
    updateMinScale();

    currentScale = 1.0;
    if(imageFitMode == FREE)
        imageFitMode = ALL;
    fitDefault();

    cropOverlay->setRealSize(sourceSize);
    cropOverlay->setImageArea(drawingRect, currentScale);
    cropOverlay->hide();

    mapOverlay->updatePosition();
    updateMap();
    if(settings->transparencyGrid())
        drawTransparencyGrid();
    update();

    connect(resizeTimer, SIGNAL(timeout()),
            this, SLOT(resizeImage()), Qt::UniqueConnection);
    resizeTimer->start(0);
    // cursorTimer->start(2000);
}

// takes scaled image
void ImageViewer::updateImage(QPixmap *scaled) {
    delete image;
    image = scaled;
    if(transparencyGridEnabled)
        drawTransparencyGrid();
    update();
}

void ImageViewer::crop() {
    disconnect(cropOverlay, SIGNAL(selected(QRect)),
               this, SIGNAL(wallpaperSelected(QRect)));
    connect(cropOverlay, SIGNAL(selected(QRect)),
            this, SIGNAL(cropSelected(QRect)), Qt::UniqueConnection);
    if(cropOverlay->isHidden() && isDisplaying()) {
        cursorTimer->stop();
        cropOverlay->setButtonText("CROP");
        cropOverlay->display();
    } else {
        cropOverlay->hide();
    }
}

void ImageViewer::selectWallpaper() {
    disconnect(cropOverlay, SIGNAL(selected(QRect)),
               this, SIGNAL(cropSelected(QRect)));
    connect(cropOverlay, SIGNAL(selected(QRect)),
            this, SIGNAL(wallpaperSelected(QRect)), Qt::UniqueConnection);
    if(cropOverlay->isHidden() && isDisplaying()) {
        cursorTimer->stop();
        cropOverlay->setButtonText("SET WALLPAPER");
        cropOverlay->display();
    } else {
        cropOverlay->hide();
    }
}

void ImageViewer::readSettings() {
    transparencyGridEnabled = settings->transparencyGrid();
    this->bgColor = settings->backgroundColor();
    this->repaint();
}

void ImageViewer::updateMaxScale() {
    if(isDisplaying()) {
        if(sourceSize.width() < width() &&
                sourceSize.height() < height()) {
            maxScale = 1;
            return;
        }
        float newMaxScaleX = (float) width() / sourceSize.width();
        float newMaxScaleY = (float) height() / sourceSize.height();
        if(newMaxScaleX < newMaxScaleY) {
            maxScale = newMaxScaleX;
        } else {
            maxScale = newMaxScaleY;
        }
    }
}

void ImageViewer::updateMinScale() {
    minScale = 3.0;
    float imgSize = sourceSize.width() * sourceSize.height() / 1000000;
    float maxSize =
        minScale * sourceSize.width() * sourceSize.height() / 1000000;
    if(maxSize > 25) {
        minScale = sqrt(25 / imgSize);
    }
}

float ImageViewer::scale() const {
    return currentScale;
}

void ImageViewer::setScale(float scale) {
    if(scale > minScale) {
        currentScale = minScale;
    } else if(scale <= maxScale + FLT_EPSILON) {
        currentScale = maxScale;
        if(imageFitMode == FREE)
            imageFitMode = ALL;
    } else {
        currentScale = scale;
    }
    float h = scale * sourceSize.height();
    float w = scale * sourceSize.width();
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
        emit scalingRequested(drawingRect.size());
    }
}

void ImageViewer::drawTransparencyGrid() {
    if(image && image->hasAlphaChannel()) {
        QPainter painter(image);
        painter.setCompositionMode(QPainter::CompositionMode_DestinationOver);
        QColor dark(90,90,90,255);
        QColor light(125,125,125,255);
        int xCount, yCount;
        xCount = image->width() / transparencyGridSize;
        yCount = image->height() / transparencyGridSize;
        QRect square(0, 0, transparencyGridSize, transparencyGridSize);
        bool evenOdd;
        for(int i = 0; i <= yCount; i++) {
            evenOdd = (i % 2);
            for(int j = 0; j <= xCount; j++) {
                if(j % 2 == evenOdd)
                    painter.fillRect(square, light);
                square.translate(transparencyGridSize, 0);
            }
            square.translate(0, transparencyGridSize);
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
    painter.fillRect(rect(), QBrush(bgColor));
    //painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.drawPixmap(drawingRect, *image, image->rect());
}

void ImageViewer::mousePressEvent(QMouseEvent *event) {
    QWidget::mousePressEvent(event);
    if(!isDisplaying()) {
        return;
    }
    mapOverlay->enableVisibility(true);
    cursorTimer->stop();
    setCursor(QCursor(Qt::ArrowCursor));
    mouseMoveStartPos = event->pos();
    if(event->button() == Qt::LeftButton) {
        this->setCursor(QCursor(Qt::ClosedHandCursor));
    }
    if(event->button() == Qt::RightButton) {
        this->setCursor(QCursor(Qt::SizeVerCursor));
        fixedZoomPoint = event->pos();
    }
}

void ImageViewer::mouseMoveEvent(QMouseEvent *event) {
    QWidget::mouseMoveEvent(event);
    cursorTimer->stop();
    if(!isDisplaying()) {
        return;
    }
    if(event->buttons() & Qt::LeftButton) {
        mouseWrapping?mouseDragWrapping(event):mouseDrag(event);
    } else if(event->buttons() & Qt::RightButton) {
        mouseZoom(event);
    } else {
        setCursor(QCursor(Qt::ArrowCursor));
        cursorTimer->start(2000);
    }
}

void ImageViewer::mouseReleaseEvent(QMouseEvent *event) {
    QWidget::mouseReleaseEvent(event);
    if(!isDisplaying()) {
        return;
    }
    mouseMoveStartPos = event->pos();
    fixedZoomPoint = event->pos();
    this->setCursor(QCursor(Qt::ArrowCursor));
    if(event->button() == Qt::RightButton && imageFitMode != ALL) {
        //resizeTimer->start(0);
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
        int left = drawingRect.x() - mouseMoveStartPos.x();
        int top = drawingRect.y() - mouseMoveStartPos.y();
        int right = left + drawingRect.width();
        int bottom = top + drawingRect.height();
        if(left <= 0 && right > size().width())
            drawingRect.moveLeft(left);
        if(top <= 0 && bottom > size().height())
            drawingRect.moveTop(top);
        mouseMoveStartPos = event->pos();
        updateMap();
        update();
    }
}

void ImageViewer::mouseZoom(QMouseEvent *event) {
    float step = (maxScale - minScale) / -500.0;
    int currentPos = event->pos().y();
    int moveDistance = mouseMoveStartPos.y() - currentPos;
    float newScale = currentScale + step * (moveDistance);
    mouseMoveStartPos = event->pos();

    if(moveDistance < 0 && currentScale <= maxScale) {
        return;
    } else if(moveDistance > 0 && newScale > minScale) { // already at max zoom
        newScale = minScale;
    } else if(moveDistance < 0 && newScale < maxScale - FLT_EPSILON) { // at min zoom
        newScale = maxScale;
        slotFitAll();
    } else {
        imageFitMode = FREE;
        scaleAround(fixedZoomPoint, newScale);
        resizeTimer->stop();
        resizeTimer->start(65);
    }
    update();
}

void ImageViewer::fitWidth() {
    if(isDisplaying()) {
        float scale = (float) width() / sourceSize.width();
        setScale(scale);
        centerImage();
        if(drawingRect.height() > height())
            drawingRect.moveTop(0);
        update();
    } else if(errorFlag) {
        fitNormal();
    } else {
        centerImage();
    }
}

void ImageViewer::fitAll() {
    if(isDisplaying()) {
        bool h = sourceSize.height() <= height();
        bool w = sourceSize.width() <= width();
        // source image fits entirely
        if(h && w) {
            fitNormal();
            return;
        } else { // doesnt fit
            setScale(maxScale);
            centerImage();
            update();
        }
    } else if(errorFlag) {
        fitNormal();
    } else {
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
        case NORMAL:
            fitNormal();
            break;
        case WIDTH:
            fitWidth();
            break;
        case ALL:
            fitAll();
            break;
        default: /* FREE etc */
            break;
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

void ImageViewer::resizeEvent(QResizeEvent *event) {
    Q_UNUSED(event)
    updateMaxScale();
    if(imageFitMode == FREE || imageFitMode == NORMAL) {
        alignImage();
    } else {
        fitDefault();
    }
    emit resized(size());
    mapOverlay->updatePosition();
    cropOverlay->hide();
    updateMap();
    update();
    resizeTimer->start(130);
}

// centers image inside window rectangle
void ImageViewer::centerImage() {
    drawingRect.moveCenter(rect().center());
}

// centers image inside window
// aligns image corner to window corner if needed
void ImageViewer::alignImage() {
    if(drawingRect.height() <= height()) {
        drawingRect.moveTop((height() - drawingRect.height()) / 2);
    } else {
        fixAlignVertical();
    }
    if(drawingRect.width() <= width()) {
        drawingRect.moveLeft((width() - drawingRect.width()) / 2);
    } else {
        fixAlignHorizontal();
    }
}

void ImageViewer::fixAlignHorizontal() {
    if(drawingRect.x() > 0 && drawingRect.right() > width()) {
        drawingRect.moveLeft(0);
    }
    if(width() - drawingRect.x() > drawingRect.width()) {
        drawingRect.moveRight(width());
    }
}

void ImageViewer::fixAlignVertical() {
    if(drawingRect.y() > 0 && drawingRect.bottom() > height()) {
        drawingRect.moveTop(0);
    }
    if(height() - drawingRect.y() > drawingRect.height()) {
        drawingRect.moveBottom(height());
    }
}

// scales image around point, so point's position
// relative to window remains unchanged
// literally shit. just center for now
void ImageViewer::scaleAround(QPointF p, float newScale) {
    float oldX = drawingRect.x();
    float xPos = (float)(p.x() - oldX) / (drawingRect.width());
    float oldPx = (float) xPos * drawingRect.width();
    float oldY = drawingRect.y();
    float yPos = (float)(p.y() - oldY) / drawingRect.height();
    float oldPy = (float) yPos * drawingRect.height();
    setScale(newScale);
    float newPx = (float) xPos * drawingRect.width();
    drawingRect.moveLeft(oldX - (newPx - oldPx));
    float newPy = (float) yPos * drawingRect.height();
    drawingRect.moveTop(oldY - (newPy - oldPy));

    setScale(newScale);
    //centerImage();
    alignImage();
}

void ImageViewer::slotZoomIn() {
    if(!isDisplaying()) {
        return;
    }
    float newScale = scale() + scaleStep;
    if(newScale == currentScale) {    //skip if minScale
        return;
    }
    if(newScale > minScale) {
        newScale = minScale;
    }
    imageFitMode = FREE;
    scaleAround(rect().center(), newScale);
    updateMap();
    update();
    resizeTimer->start(90);
}

void ImageViewer::slotZoomOut() {
    if(!isDisplaying()) {
        return;
    }
    float newScale = scale() - scaleStep;
    if(newScale == currentScale)    //skip if maxScale
        return;
    if(newScale < maxScale - FLT_EPSILON) {
        newScale = maxScale;
    }
    imageFitMode = FREE;
    scaleAround(rect().center(), newScale);
    updateMap();
    update();
    resizeTimer->start(90);
}

bool ImageViewer::isDisplaying() const {
    return isDisplayingFlag;
}

void ImageViewer::hideCursor() {
    cursorTimer->stop();
    setCursor(QCursor(Qt::BlankCursor));
}
