#include "imageviewer.h"

ImageViewer::ImageViewer(): QWidget(),
    currentImage(NULL),
    isDisplayingFlag(false),
    errorFlag(false),
    currentScale(1.0),
    maxScale(1.0),
    minScale(3.0),
    scaleStep(0.1),
    imageFitMode(NORMAL)
{
    initMap();
    bgColor.setRgb(17,17,17,255);
    image = new QImage();
    image->load(":/images/res/logo.png");
    drawingRect = image->rect();
    fitDefault();
    resizeTimer = new QTimer(this);
    connect(resizeTimer, SIGNAL(timeout()),
            this, SLOT(resizeImage()),
            Qt::UniqueConnection);
}

ImageViewer::ImageViewer(QWidget* parent): QWidget(parent),
    currentImage(NULL),
    isDisplayingFlag(false),
    errorFlag(false),
    currentScale(1.0),
    maxScale(1.0),
    minScale(4.0),
    scaleStep(0.1),
    imageFitMode(NORMAL)
{
    initMap();
    bgColor.setRgb(17,17,17,255);
    image = new QImage();
    image->load(":/images/res/logo.png");
    drawingRect = image->rect();
    resizeTimer = new QTimer(this);
    connect(resizeTimer, SIGNAL(timeout()),
            this, SLOT(resizeImage()),
            Qt::UniqueConnection);
}

ImageViewer::~ImageViewer() {
    delete currentImage;
}

void ImageViewer::initMap() {
    mapOverlay = new MapOverlay(this);
}

bool ImageViewer::imageIsScaled() const {
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
    disconnect(resizeTimer, SIGNAL(timeout()), this, SLOT(resizeImage()));
    connect(currentImage->getMovie(), SIGNAL(frameChanged(int)),
            this, SLOT(onAnimation()),Qt::DirectConnection);
    currentImage->getMovie()->start();
}

void ImageViewer::onAnimation() {
    *image = currentImage->getMovie()->currentImage();
    update();
}

void ImageViewer::freeImage() {
    if (currentImage!=NULL) {
        stopAnimation();
        currentImage->setUseFlag(false);
    }
}

void ImageViewer::displayImage(Image* i) {
    resizeTimer->stop();
    currentImage = i;
    if(i->getType()==NONE) {
        //empty or corrupted image
        image->load(":/images/res/error.png");
        isDisplayingFlag = false;
        errorFlag=true;
    }
    else {
        errorFlag=false;
        isDisplayingFlag = true;
        if(currentImage->getType() == STATIC) {
           *image = *currentImage->getImage();
            connect(resizeTimer, SIGNAL(timeout()),
                    this, SLOT(resizeImage()),Qt::UniqueConnection);
        }
        else if (currentImage->getType() == GIF) {
            currentImage->getMovie()->jumpToFrame(0);
            *image = currentImage->getMovie()->currentImage();
            startAnimation();
        }
        updateMaxScale();
        updateMinScale();
    }
    drawingRect = image->rect();
    currentScale = 1.0;
    if(imageFitMode == FREE)
        imageFitMode = ALL;
    fitDefault();
    resizeTimer->start(15);
    emit imageChanged();
}

void ImageViewer::updateMaxScale() {
    if(isDisplaying()) {
        if (currentImage->width() < width() &&
            currentImage->height() < height()) {
            maxScale = 1;
            return;
        }
        float newMaxScaleX = (float)width()/currentImage->width();
        float newMaxScaleY = (float)height()/currentImage->height();
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
    float imgSize = currentImage->width()*currentImage->height()/1000000;
    float maxSize =
            minScale*currentImage->width()*currentImage->height()/1000000;
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
            imageFitMode = ALL; // TODO: update gui checkbox
        }
        else {
            currentScale = scale;
        }
        float h = scale*currentImage->height();
        float w = scale*currentImage->width();
        drawingRect.setHeight(h);
        drawingRect.setWidth(w);
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
        float size = drawingRect.width()*drawingRect.height()/1000000;

        int time = clock();

        delete image;
        image = new QImage(drawingRect.size(),QImage::Format_ARGB32_Premultiplied);
        image->fill(qRgba(0,0,0,0));

        // ######## QPainter scale ########## faster but looks kinda crappy
        bool goPainter = false;
        if(goPainter)
        {
            bool smoothEnabled;
            size>=15?smoothEnabled=false:smoothEnabled=true;
            QPainter painter(image);
            painter.setRenderHint(QPainter::SmoothPixmapTransform, smoothEnabled);
            painter.drawImage(QRectF(QPointF(0,0),
                              drawingRect.size()),
                              *currentImage->getImage(),
                              currentImage->getImage()->rect()
                              );
        } else {

        // ############ END #############

        // ###### QImage-based scale ###### like 10x slower
        Qt::TransformationMode mode;
        size>=15?mode=Qt::FastTransformation:mode=Qt::SmoothTransformation;
        *image = currentImage->getImage()->scaled(drawingRect.width(),
                                                 drawingRect.height(),
                                                 Qt::IgnoreAspectRatio,
                                                 mode);
        }
        // ############ END #############

        //########## save image to ~/compare
          image->save("/home/mitcher/compare/out.jpg", "JPG", 100);

        qDebug() << "###### scaling ######";
   //     qDebug() << "res: " << drawingRect.width() << "x" << drawingRect.height();
     //   qDebug() << size << "  scale: " <<  currentScale;
        qDebug() << "time: " << clock()-time;
        qDebug() << "#####################";

        update();
    }
}

// ##################################################
// ####################  PAINT  #####################
// ##################################################
void ImageViewer::paintEvent(QPaintEvent* event) {
    Q_UNUSED( event )
    QPainter painter(this);
    painter.fillRect(rect(), QBrush(bgColor));
    if(currentImage && currentImage->getType() == GIF) {
        painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    }
    //int time = clock();
        qDebug() << "drawing " << image->size() << " image on " << drawingRect.size() << " surface.";
    painter.drawImage(drawingRect, *image, image->rect());
    //qDebug() << "VIEWER: draw time: " << clock() - time;
}

void ImageViewer::mousePressEvent(QMouseEvent* event) {
    if(!isDisplaying()) {
        return;
    }
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
    if (event->buttons() & Qt::RightButton) {
        float step = (maxScale - minScale) / -500.0;
        int currentPos = event->pos().y();
        int moveDistance = mouseMoveStartPos.y() - currentPos;
        float newScale = currentScale + step*(moveDistance);
        mouseMoveStartPos = event->pos();

        if(moveDistance < 0 && currentScale <= maxScale) {
            return;
        } else if(moveDistance > 0 && newScale > minScale) { // already at max zoom
                newScale = minScale;
                return;
        } else if(moveDistance < 0 && newScale < maxScale-FLT_EPSILON) { // a min zoom
                newScale = maxScale;
                slotFitAll();
                return;
        }
        imageFitMode = FREE;
        scaleAround(fixedZoomPoint, newScale);
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
        resizeTimer->start(0);
        fitDefault();
    }
}

void ImageViewer::fitWidth() {
    if(isDisplaying()) {
        float scale = (float) width() / currentImage->size().width();
        setScale(scale);
        imageAlign();
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
        bool h = currentImage->height() <= height();
        bool w = currentImage->width() <= width();
        // source image fits entirely
        if(h && w) {
            fitNormal();
            return;
        }
        else { // doesnt fit
                setScale(maxScale);
                imageAlign();
                update();
        }
    }
    else if(errorFlag) {
        fitNormal();
    }
    else {
        imageAlign();
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
        default: /* FREE etc */ break;
    }
    updateMap();
}

void ImageViewer::updateMap() {
    mapOverlay->updateMap(size(), drawingRect);
}

void ImageViewer::slotFitNormal() {
    imageFitMode = NORMAL;
    fitDefault();
    resizeTimer->start(0);
}

void ImageViewer::slotFitWidth() {
    imageFitMode = WIDTH;
    fitDefault();
    resizeTimer->start(0);
}

void ImageViewer::slotFitAll() {
    imageFitMode = ALL;
    fitDefault();
    resizeTimer->start(0);
}

void ImageViewer::resizeEvent(QResizeEvent* event) {
    Q_UNUSED( event )

    updateMaxScale();
    if(imageFitMode == FREE || imageFitMode == NORMAL) {
        imageAlign();
    }
    else {
        fitDefault();
    }
    mapOverlay->updateMap(size(),drawingRect);
    mapOverlay->updatePosition();
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

// scales image around point, so point's position
// relative to window remains unchanged
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
    resizeTimer->start(100);
}

Image* ImageViewer::getCurrentImage() const {
    return currentImage;
}

bool ImageViewer::isDisplaying() const {
    return isDisplayingFlag;
}
