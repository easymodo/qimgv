#include "imageviewer.h"

#include <qgraphicseffect.h>
#define AVG(a,b)  ( ((((a)^(b)) & 0xfefefefeUL) >> 1) + ((a)&(b)) )

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
    void centerVertical();
    void centerHorizontal();
    void setImage(Image* image);
    void setScale(float scale);
    void scaleImage();
    float scale() const;
    bool scaled() const;
    void smoothScale();
    QImage halfSized(const QImage &source);

    Image* img;
    QTimer animationTimer;
    QImageReader imageReader;
    QImage image, imageScaled;
    QPoint cursorMovedDistance;
    QRect drawingRect;
    QSize shrinkSize;
    MapOverlay *mapOverlay;
    InfoOverlay *infoOverlay;
    ControlsOverlay *controlsOverlay;
    QFuture<void> scalerThread;
    ImageViewer* q;
    QMutex mutex;
    uint lock;

    WindowResizePolicy resizePolicy;

    int freeSpaceLeft;
    int freeSpaceBottom;
    int freeSpaceRight;
    int freeSpaceTop;

    bool isDisplaying;

    static const float maxScale = 0.20;
    static const float minScale = 3.0;
    static const float zoomStep = 0.1;

private:
    float currentScale;
};

ImageViewerPrivate::ImageViewerPrivate(ImageViewer* qq)
    : q(qq),
      shrinkSize(),
      currentScale(1.0),
      resizePolicy(NORMAL),
      img(NULL),
      isDisplaying(false),
      lock(0)
{
    infoOverlay = new InfoOverlay(q);
    mapOverlay = new MapOverlay(q);
    controlsOverlay = new ControlsOverlay(q);
    infoOverlay->hide();
    controlsOverlay->hide();
}

ImageViewerPrivate::~ImageViewerPrivate()
{
    delete img;
}

bool ImageViewerPrivate::scaled() const
{
    return scale() != 1.0;
}

void ImageViewerPrivate::setImage(Image* i) {
    if (img!=NULL) {
        if(img->getType()==GIF) {
            img->getMovie()->stop();
            q->disconnect(img->getMovie(), SIGNAL(frameChanged(int)), q, SLOT(onAnimation()));
        }
//         delete img;
//         img = NULL;
    }

    if(i->getType()==NONE) {
        //empty or corrupted image
        image.load(":/images/res/error.png");
        isDisplaying = false;
    }
    else {
        //ok, proceeding
        isDisplaying = true;
        img = i;
        if(img->getType() == STATIC) {
            image = *img->getImage();
        }
        else if (img->getType() == GIF) {
            img->getMovie()->jumpToFrame(0);
            image = img->getMovie()->currentImage();
            q->connect(img->getMovie(), SIGNAL(frameChanged(int)), q, SLOT(onAnimation()));
            img->getMovie()->start();
        }
        emit q->imageChanged();
    }
    drawingRect = image.rect();
    if(resizePolicy == FREE)
        resizePolicy = NORMAL;
}

float ImageViewerPrivate::scale() const
{
    return currentScale;
}

void ImageViewerPrivate::setScale(float scale)
{
    currentScale = scale;
    QSize sz;
    sz = image.size();
    sz = sz.scaled(sz * scale, Qt::KeepAspectRatio); // for qt's
    drawingRect.setSize(sz);
}

void ImageViewerPrivate::smoothScale() {
    lock++;
    Sleeper::msleep(100); //unicorn magic. prevents some bad things
    if(lock == 1) {
        qDebug() << "enter";
        imageScaled = image.scaled(drawingRect.size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation); //SLOW
        q->update();
        qDebug() << "quit";
    }
    lock--;
}

/*QImage ImageViewerPrivate::halfSized(const QImage &source)
{
    QImage dest(source.size() * 0.5, QImage::Format_ARGB32_Premultiplied);

    const quint32 *src = reinterpret_cast<const quint32*>(source.bits());
    int sx = source.bytesPerLine() >> 2;
    int sx2 = sx << 1;

    quint32 *dst = reinterpret_cast<quint32*>(dest.bits());
    int dx = dest.bytesPerLine() >> 2;
    int ww = dest.width();
    int hh = dest.height();

    for (int y = hh; y; --y, dst += dx, src += sx2) {
        const quint32 *p1 = src;
        const quint32 *p2 = src + sx;
        quint32 *q = dst;
        for (int x = ww; x; --x, q++, p1 += 2, p2 += 2)
            * q = AVG(AVG(p1[0], p1[1]), AVG(p2[0], p2[1]));
    }
    return dest;
}
*/

void ImageViewerPrivate::scaleImage()
{
    if(scaled() && scale() <= 1.4) {
        imageScaled = image.scaled(drawingRect.size(), Qt::IgnoreAspectRatio);
        //smoothing + gif = lags
        if(isDisplaying && img->getType() == STATIC)
            QFuture<void> t1 = QtConcurrent::run(this, &ImageViewerPrivate::smoothScale);
    }
    q->update();
}

void ImageViewerPrivate::centerHorizontal()
{
    QPoint point((q->width() - drawingRect.width()) / 2, drawingRect.y());
    drawingRect.moveTo(point);
}

void ImageViewerPrivate::centerVertical()
{
    QPoint point(drawingRect.x(), (q->height() - drawingRect.height()) / 2);
    drawingRect.moveTo(point);
}

ImageViewer::ImageViewer(): QWidget(), d(new ImageViewerPrivate(this))
{
    d->image.load(":/images/res/logo.png");
    d->drawingRect = d->image.rect();
    fitDefault();
}

ImageViewer::ImageViewer(QWidget* parent): QWidget(parent),
d(new ImageViewerPrivate(this))
{
    d->image.load(":/images/res/logo.png");
    d->drawingRect = d->image.rect();
}

ImageViewer::ImageViewer(QWidget* parent, Image* image) : QWidget(parent),
d(new ImageViewerPrivate(this))
{
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
    d->scaleImage();
    update();
}

void ImageViewer::paintEvent(QPaintEvent* event)
{
    QColor bgColor = QColor(0,0,0,255);
    QPainter painter(this);
    painter.setPen(bgColor);
    painter.setBrush(Qt::SolidPattern);
    painter.drawRect(QRect(0,0,this->width(),this->height()));

    int time = clock();
    if(d->scaled() && d->scale() <= 1.4) {
        //qDebug() << d->drawingRect << " <_> " << d->imageScaled.size();
        painter.drawImage(d->drawingRect, d->imageScaled);
    }
    else {
        painter.drawImage(d->drawingRect, d->image);
    }
    //qDebug() << "draw time: " << clock() - time;
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
        if(d->drawingRect.size().width() > this->width() ||
           d->drawingRect.size().height() > this->height()) {
           // qDebug() << d->drawingRect;
           // qDebug() << this->size();
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
            d->mapOverlay->updateMap(size(),d->drawingRect);
        }
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
    float scale = (float) width() / d->image.width();
    d->drawingRect.setX(0);
    d->setScale(scale);
    if(d->drawingRect.height()<=height()) {
        QPoint point(0, (height() - d->drawingRect.height()) / 2);
        d->drawingRect.moveTo(point);
    }
    else
        d->drawingRect.moveTop(0);
    d->scaleImage();
}

void ImageViewer::fitHorizontal()
{
    float scale = (float) width() / d->image.width();
    d->drawingRect.setX(0);
    d->setScale(scale);
    d->centerVertical();
}

void ImageViewer::fitVertical()
{
    float scale = (float) height() / d->image.height();
    d->drawingRect.setY(0);
    d->setScale(scale);
    d->centerHorizontal();
}

void ImageViewer::fitAll()
{
    bool h = d->image.height() <= this->height();
    bool w = d->image.width() <= this->width();
    if(h && w) {
        fitOriginal();
    }
    else {
        QSize oldSize = d->drawingRect.size();
        float widgetAspect = (float) height() / width();
        float drawingAspect = (float) d->drawingRect.height() /
                d->drawingRect.width();
        if(widgetAspect < drawingAspect)
            fitVertical();
        else
            fitHorizontal();
        if(d->scaled() && oldSize != d->drawingRect.size()) {
            d->scaleImage();
        }
    }
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
    d->mapOverlay->updateMap(size(),d->drawingRect);
}

void ImageViewer::centerImage() {
    int left = d->drawingRect.left()<0?0:d->drawingRect.left();
    int right = d->drawingRect.right()<0?0:d->drawingRect.right();
    int top = d->drawingRect.top()<0?0:d->drawingRect.top();
    int bottom = d->drawingRect.bottom()<0?0:d->drawingRect.bottom();
    int spaceLeft = left - rect().left();
    int spaceTop = top - rect().top();
    int spaceRight = rect().right() - right;
    int spaceBottom = rect().bottom() - bottom;

    if (spaceLeft < 0 && spaceRight > 0)
        d->drawingRect.translate(spaceRight, 0);
    else if (spaceLeft > 0 || spaceRight > 0)
        d->centerHorizontal();

    if (spaceTop < 0 && spaceBottom > 0)
        d->drawingRect.translate(0, spaceBottom);
    else if (spaceTop > 0 || spaceBottom > 0)
        d->centerVertical();
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
    fitDefault();
    d->mapOverlay->updateMap(size(),d->drawingRect);
    d->controlsOverlay->updateSize();
    d->mapOverlay->updatePosition();
}

void ImageViewer::mouseDoubleClickEvent(QMouseEvent *event) {
    emit sendDoubleClick();
}

void ImageViewer::slotZoomIn() {
    d->resizePolicy = FREE;
    if(d->isDisplaying) {
        if(d->scale() == d->minScale)
            return;
        float possibleScale = d->scale() + d->zoomStep;
        if (possibleScale <= d->minScale) {
            d->setScale(possibleScale);
        }
        else {
            d->setScale(d->minScale);
        }
        d->centerHorizontal();
        d->centerVertical();
        d->scaleImage();
        d->mapOverlay->updateMap(size(), d->drawingRect);
    }
}

void ImageViewer::slotZoomOut() {
    d->resizePolicy = FREE;
    if(d->isDisplaying) {
        if(d->scale() == d->maxScale)
            return;
        float possibleScale = d->scale() - d->zoomStep;
        if (possibleScale >= d->maxScale) {
            d->setScale(possibleScale);
        }
        else {
            d->setScale(d->maxScale);
        }
        d->centerHorizontal();
        d->centerVertical();
        d->scaleImage();
        d->mapOverlay->updateMap(size(), d->drawingRect);
    }
}

Image* ImageViewer::getImage() const {
    return d->img;
}

void ImageViewer::setScale(float scale)
{
    d->setScale(scale);
}

void ImageViewer::slotSetInfoString(QString info) {
    d->infoOverlay->setText(info);
}

void ImageViewer::slotShowInfo(bool x) {
    x?d->infoOverlay->show():d->infoOverlay->hide();
}

void ImageViewer::slotShowControls(bool x) {
    x?d->controlsOverlay->show():d->controlsOverlay->hide();
}

ControlsOverlay* ImageViewer::getControls() {
    return d->controlsOverlay;
}

bool ImageViewer::isDisplaying() {
    return d->isDisplaying;
}
