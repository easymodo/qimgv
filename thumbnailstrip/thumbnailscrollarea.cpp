/*  QUALITY code here
 *
 */

#include "thumbnailscrollarea.h"
#include "thumbnaillabel.h"

ThumbnailScrollArea::ThumbnailScrollArea(QWidget *parent) :
    QScrollArea(parent),
    scrollStep(200),
    defaultHeight(125),
    loadDelay(120)
{
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    loadTimer.setSingleShot(true);
  //  setFrameShape(QFrame::NoFrame);
    this->setMinimumHeight(defaultHeight);
    qDebug() << parent->size();
    this->setGeometry(0,0, parent->width(), defaultHeight);
    this->hide();
    strip = new ThumbnailStrip(this);
    strip->setMinimumHeight(this->minimumHeight());
    this->setWidgetResizable(true);
    this->setWidget(strip);
    updateVisibleRegion();

    connect(strip, SIGNAL(thumbnailClicked(int)),
            this, SIGNAL(thumbnailClicked(int)));
    connect(this->horizontalScrollBar(), SIGNAL(valueChanged(int)),
            this, SLOT(loadVisibleThumbnailsDelayed()));
    connect(this->horizontalScrollBar(), SIGNAL(sliderMoved(int)),
            this, SLOT(loadVisibleThumbnailsDelayed()));
    connect(&loadTimer, SIGNAL(timeout()), this, SLOT(loadVisibleThumbnails()));
}

void ThumbnailScrollArea::cacheInitialized(int count) {
    strip->populate(count);
    loadVisibleThumbnails();
}

void ThumbnailScrollArea::wheelEvent(QWheelEvent *event) {
    if(event->angleDelta().ry() < 0) {
        horizontalScrollBar()->
                setValue(horizontalScrollBar()->value()+scrollStep);
    }
    else {
        horizontalScrollBar()->
                setValue(horizontalScrollBar()->value()-scrollStep);
    }
    event->accept();
}

void ThumbnailScrollArea::loadVisibleThumbnailsDelayed() {
    loadTimer.stop();
    loadTimer.start(loadDelay);
}


void ThumbnailScrollArea::loadVisibleThumbnails() {
    qDebug() << "load";
    loadTimer.stop();
    updateVisibleRegion();
    qDebug() << "visible: " << visibleRegion;
    int counter = 0;
    for(int i=0; i<strip->thumbnailLabels.count(); i++) {
        if(strip->thumbnailLabels.at(i)->state == EMPTY  &&
            childVisible(strip->thumbnailLabels.at(i)))
        {
            strip->thumbnailLabels.at(i)->state = LOADING;
            counter++;
            emit thumbnailRequested(i);
        }
    }
    qDebug() << "loaded " << counter << " items.";
}

void ThumbnailScrollArea::setThumbnail(int pos, const QPixmap* thumb) {
    strip->thumbnailLabels.at(pos)->setPixmap(*thumb);
    strip->thumbnailLabels.at(pos)->state = LOADED;
}

void ThumbnailScrollArea::updateVisibleRegion() {
    visibleRegion = rect();
    visibleRegion.moveLeft(this->horizontalScrollBar()->value());
    visibleRegion.adjust(-350,0,350,0);
}

bool ThumbnailScrollArea::childVisible(ThumbnailLabel *label) {
    return this->visibleRegion.intersects(label->relativeRect());
}

void ThumbnailScrollArea::mouseMoveEvent(QMouseEvent* event) {
    event->accept();
}

void ThumbnailScrollArea::parentResized(QSize parentSize) {
    this->setGeometry( QRect(0, parentSize.height() - defaultHeight + 1,
                             parentSize.width(), defaultHeight) );
}

void ThumbnailScrollArea::leaveEvent(QEvent *event) {
    Q_UNUSED(event)
    this->hide();
}

ThumbnailScrollArea::~ThumbnailScrollArea() {

}

