#include "imageviewer.h"

#include <qgraphicseffect.h>

enum WindowResizePolicy
{
    NORMAL,
    WIDTH,
    ALL
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
    : q(qq), shrinkSize(), currentScale(1.0), resizePolicy(ALL), img(NULL) { }

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

//holy shit
//why
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
        bool alreadyMaxZoomOut = d->resizePolicy == ALL;
        if (alreadyMaxZoomOut)
            return;

        QSize s = d->drawingRect.size();
        bool insideWindow = s.width() <= width() || s.height() <= height();

        if (possibleScale <= 0.1 || insideWindow)
        {
            d->resizePolicy = ALL;
            fitDefault();
            return;
        }
    }

    d->setScale(possibleScale);
    d->resizePolicy = NORMAL;

    /*
    *     TODO: scale image to cursor position because
    *     current zoom reset point that cursor are looking
    */

    d->centreHorizontal();
    d->centreVertical();

    update();
}

void ImageViewer::fitWidth()
{
    double scale = (double) width() / d->image.width();
    d->drawingRect.setX(0);
    d->setScale(scale);
    d->centreVertical();
    update();
}

void ImageViewer::fitHorizontal()
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

void ImageViewer::fitVertical()
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
    d->drawingRect.setSize(d->image.size());
    d->zoomAndSizeChanged();
    update();
}

void ImageViewer::fitDefault() {
    switch(d->resizePolicy) {
        case NORMAL: fitOriginal(); break;
        case WIDTH: fitWidth(); break;
        case ALL: fitAll(); break;
    }
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
    //Bug: doesnt work when maximizing window
    resize(event->size());
    fitDefault();
}

void ImageViewer::mouseDoubleClickEvent(QMouseEvent *event) {
    emit sendDoubleClick();
}

void ImageViewer::slotZoomIn() {
    increaseScale(-0.1);
}

void ImageViewer::slotZoomOut() {
    increaseScale(0.1);
}


void ImageViewer::setScale(double scale)
{
    d->setScale(scale);
}
