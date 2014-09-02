#include "imageviewer.h"

#include <qgraphicseffect.h>

enum WindowResizePolicy
{
    NORMAL,
    WIDTH,
    ALL,
    FREE
};

class ImageViewerPrivate
{
public:
    ImageViewerPrivate(ImageViewer* qq);
    ~ImageViewerPrivate();
    void centreVertical();
    void centreHorizontal();
    //void zoomAndSizeChanged();
    void setImage(Image* image);
    void setScale(double scale);
    double scale() const;
    bool scaled() const;

    Image* img;
    QTimer animationTimer;
    QImageReader imageReader;
    QImage image;
    QPoint cursorMovedDistance;
    QRect drawingRect;
    QSize shrinkSize;
    ImageViewer* q;

    WindowResizePolicy resizePolicy;

    int freeSpaceLeft;
    int freeSpaceBottom;
    int freeSpaceRight;
    int freeSpaceTop;

    static const double maxScale = 0.10;
    static const double minScale = 3.0;
    static const double zoomStep = 0.1;

private:
    double currentScale;
};

ImageViewerPrivate::ImageViewerPrivate(ImageViewer* qq)
    : q(qq), shrinkSize(), currentScale(1.0), resizePolicy(NORMAL), img(NULL) { }

ImageViewerPrivate::~ImageViewerPrivate()
{
    delete img;
}

bool ImageViewerPrivate::scaled() const
{
    return scale() != 1.0;
}


void ImageViewerPrivate::setImage(Image* i) {
  // add stop movie if prev img was gif
    if (img!=NULL && img->getType()==GIF) {
        img->getMovie()->stop();
        q->disconnect(img->getMovie(), SIGNAL(frameChanged(int)), q, SLOT(onAnimation()));
    }
    img = i;
    if(img->getType() == STATIC) {
        image = *img->getImage();
        q->update();
    }
    else if (img->getType() == GIF) {
        img->getMovie()->jumpToNextFrame();
        image = img->getMovie()->currentImage();
        q->connect(img->getMovie(), SIGNAL(frameChanged(int)), q, SLOT(onAnimation()));
        img->getMovie()->start();
    }
    drawingRect = image.rect();
}

double ImageViewerPrivate::scale() const
{
    return currentScale;
}

void ImageViewerPrivate::setScale(double scale)
{
    currentScale = scale;
    QSize sz = image.size();
    sz = sz.scaled(sz * scale, Qt::KeepAspectRatio);
    drawingRect.setSize(sz);
}
/*
void ImageViewerPrivate::zoomAndSizeChanged()
{
    setScale(1.0);
    int spaceLeft = drawingRect.left() - q->rect().left();
    int spaceTop = drawingRect.top() - q->rect().top();
    int spaceRight = q->rect().right() - drawingRect.right();
    int spaceBottom = q->rect().bottom() - drawingRect.bottom();

    if (spaceLeft < 0 && spaceRight > 0)
        drawingRect.translate(spaceRight, 0);
    else if (spaceLeft > 0 || spaceRight > 0)
        centreHorizontal();

    if (spaceTop < 0 && spaceBottom > 0)
        drawingRect.translate(0, spaceBottom);
    else if (spaceTop > 0 || spaceBottom > 0)
        centreVertical();
}
*/

void ImageViewerPrivate::centreHorizontal()
{
    QPoint point((q->width() - drawingRect.width()) / 2, drawingRect.y());
    drawingRect.moveTo(point);
}

void ImageViewerPrivate::centreVertical()
{
    QPoint point(drawingRect.x(), (q->height() - drawingRect.height()) / 2);
    drawingRect.moveTo(point);
}

ImageViewer::ImageViewer(): QWidget(), d(new ImageViewerPrivate(this))
{
    QColor bgColor = QColor(0,0,0,255);
    QPalette bg(bgColor);
    this->setPalette(bg);
    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    this->setSizePolicy(sizePolicy);
}

ImageViewer::ImageViewer(QWidget* parent): QWidget(parent),
d(new ImageViewerPrivate(this))
{
    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    this->setSizePolicy(sizePolicy);
}

ImageViewer::ImageViewer(QWidget* parent, Image* image) : QWidget(parent),
d(new ImageViewerPrivate(this))
{
    QColor bgColor = QColor(0,0,0,255);
    QPalette bg(bgColor);
    this->setPalette(bg);
    setImage(image);
}

ImageViewer::~ImageViewer()
{
    delete d;
}

void ImageViewer::setImage(Image* image)
{
    d->setImage(image);
    fitDefault();
}

void ImageViewer::onAnimation()
{
    d->image = d->img->getMovie()->currentImage();
    update();
}

void ImageViewer::paintEvent(QPaintEvent* event)
{
    QColor bgColor = QColor(0,0,0,255);
    QPainter painter(this);
    painter.setPen(bgColor);
    painter.setBrush(Qt::SolidPattern);
    painter.drawRect(QRect(0,0,this->width(),this->height()));

    painter.drawImage(d->drawingRect, d->image);
}

void ImageViewer::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        this->setCursor(QCursor(Qt::ClosedHandCursor));
        d->cursorMovedDistance = event->pos();
    }

}

void ImageViewer::mouseMoveEvent(QMouseEvent* event)
{
    if (event->buttons() & Qt::LeftButton) {
        d->cursorMovedDistance -= event->pos();

        int left = d->drawingRect.x() - d->cursorMovedDistance.x();
        int top = d->drawingRect.y() - d->cursorMovedDistance.y();
        int right = left + d->drawingRect.width();
        int bottom = top + d->drawingRect.height();

        if (left < 0 && right > size().width())
            d->drawingRect.moveLeft(left);

        if (top < 0 && bottom > size().height())
            d->drawingRect.moveTop(top);

        d->cursorMovedDistance = event->pos();
        update();
    }
}

void ImageViewer::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        this->setCursor(QCursor(Qt::ArrowCursor));
        d->cursorMovedDistance = event->pos();
    }
}

void ImageViewer::fitWidth()
{
    fitHorizontal();
    update();
}

void ImageViewer::fitHorizontal()
{
    double scale = (double) width() / d->image.width();
    d->drawingRect.setX(0);
    d->setScale(scale);
    d->centreVertical();
}

void ImageViewer::fitVertical()
{
    double scale = (double) height() / d->image.height();
    d->drawingRect.setY(0);
    d->setScale(scale);
    d->centreHorizontal();
}

void ImageViewer::fitAll()
{
    double widgetAspect = (double) height() / width();
    double drawingAspect = (double) d->drawingRect.height() /
                                                    d->drawingRect.width();
    if (widgetAspect < drawingAspect)
        fitVertical();
    else
        fitHorizontal();
    update();
}

void ImageViewer::fitOriginal()
{
    setScale(1.0);
    centerImage();
    update();
}

void ImageViewer::fitDefault() {
    switch(d->resizePolicy) {
        case NORMAL: fitOriginal(); break;
        case WIDTH: fitWidth(); break;
        case ALL: fitAll(); break;
        default: centerImage(); break;
    }
}

void ImageViewer::centerImage() {
    d->centreVertical();
    d->centreHorizontal();
}

void ImageViewer::slotFitNormal() {
    d->resizePolicy = NORMAL;
    fitDefault();
}

void ImageViewer::slotFitWidth() {
    d->resizePolicy = WIDTH;
    fitDefault();
}

void ImageViewer::slotFitAll() {
    d->resizePolicy = ALL;
    fitDefault();
}

void ImageViewer::resizeEvent(QResizeEvent* event)
{
    resize(event->size());
    if(d->resizePolicy==NORMAL) {
        update();
    } else {
        fitDefault();
    }
}

void ImageViewer::mouseDoubleClickEvent(QMouseEvent *event) {
    emit sendDoubleClick();
}

void ImageViewer::slotZoomIn() {
    double possibleScale = d->scale() + d->zoomStep;
    if (possibleScale <= d->minScale) {
        d->setScale(possibleScale);
    }
    else {
        d->setScale(d->minScale);
    }
    d->centreHorizontal();
    d->centreVertical();
    d->resizePolicy = FREE;
    update();
}

void ImageViewer::slotZoomOut() {
    double possibleScale = d->scale() - d->zoomStep;
    if (possibleScale >= d->maxScale) {
        d->setScale(possibleScale);
    }
    else {
        d->setScale(d->maxScale);
    }
    d->centreHorizontal();
    d->centreVertical();
    d->resizePolicy = FREE;
    update();
}

void ImageViewer::setScale(double scale)
{
    d->setScale(scale);
}
