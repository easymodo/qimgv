#include "imageviewer.h"

#include <qgraphicseffect.h>

enum WindowResizePolicy
{
    FIT,
    ZOOM
};

class ImageViewerPrivate
{
public:
    ImageViewerPrivate(ImageViewer* qq);
    ~ImageViewerPrivate();
    void centreVertical();
    void centreHorizontal();
    void zoomAndSizeChanged();
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

    static const double maxScale = 2.5;

private:
    double currentScale;
};

ImageViewerPrivate::ImageViewerPrivate(ImageViewer* qq)
    : q(qq), shrinkSize(), currentScale(1.0), resizePolicy(ZOOM), img(NULL) { }

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

void ImageViewerPrivate::zoomAndSizeChanged()
{
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
    QColor bgColor = QColor(0,0,0,255);
    QPalette bg(bgColor);
    this->setPalette(bg);
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
}

void ImageViewer::onAnimation()
{
    d->image = d->img->getMovie()->currentImage();
    update();
}

void ImageViewer::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.drawImage(d->drawingRect, d->image);

 /*   int ax, ay, aw, ah;
    ax=ay=aw=ah=0;
    ax=-d->drawingRect.x();
    ay=-d->drawingRect.y();
    painter.drawImage(0, 0, d->image, ax, ay, this->size().width()-ax, this->size().height()-ay);
    */
}

void ImageViewer::mousePressEvent(QMouseEvent* event)
{
    d->cursorMovedDistance = event->pos();
}

void ImageViewer::mouseMoveEvent(QMouseEvent* event)
{
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

void ImageViewer::mouseReleaseEvent(QMouseEvent* event)
{
    d->cursorMovedDistance = event->pos();
}

void ImageViewer::increaseScale(double value)
{
    bool zoomInto = value > 0;
    bool zoomOut = value < 0;
    double possibleScale = d->scale() + value;

    if (zoomInto)
    {
        if (possibleScale >= d->maxScale)
            possibleScale = d->maxScale;
    }

    if (zoomOut)
    {
        bool alreadyMaxZoomOut = d->resizePolicy == FIT;
        if (alreadyMaxZoomOut)
            return;

        QSize s = d->drawingRect.size();
        bool insideWindow = s.width() <= width() || s.height() <= height();

        if (possibleScale <= 0.1 || insideWindow)
        {
            d->resizePolicy = FIT;
            fitImageDefault();
            return;
        }
    }

    d->setScale(possibleScale);
    d->resizePolicy = ZOOM;

    /*
    *     TODO: scale image to cursor position because
    *     current zoom reset point that cursor are looking
    */

    d->centreHorizontal();
    d->centreVertical();

    update();
}

void ImageViewer::fitImageHorizontal()
{
    double scale = (double) width() / d->image.width();
    if (scale > 0 && scale < 1.0)
    {
        d->drawingRect.setX(0);
        d->setScale(scale);
    }
    else
    {
        d->centreHorizontal();
    }

    d->centreVertical();
}

void ImageViewer::fitImageVertical()
{
    double scale = (double) height() / d->image.height();
    if (scale > 0 && scale < 1.0)
    {
        d->drawingRect.setY(0);
        d->setScale(scale);
    }
    else
    {
        d->centreVertical();
    }

    d->centreHorizontal();
}

void ImageViewer::fitImageDefault()
{
    double widgetAspect = (double) height() / width();
    double drawingAspect = (double) d->drawingRect.height() /
                                                    d->drawingRect.width();

    if (widgetAspect < drawingAspect)
        fitImageVertical();
    else
        fitImageHorizontal();

    update();
}

void ImageViewer::fitImageOriginal()
{
    d->drawingRect.setSize(d->image.size());
}

void ImageViewer::resizeEvent(QResizeEvent* event)
{
    resize(event->size());

    if (d->resizePolicy == ZOOM)
        d->zoomAndSizeChanged();
    else if (d->resizePolicy == FIT)
        fitImageDefault();
}

void ImageViewer::wheelEvent(QWheelEvent* event)
{
    double forward = event->angleDelta().y();

/*
*  TODO: set non const divident {10000}
*/

    increaseScale(forward / 1200);
    event->accept();
}

void ImageViewer::setScale(double scale)
{
    d->setScale(scale);
}
