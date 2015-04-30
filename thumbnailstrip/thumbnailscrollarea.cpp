#include "thumbnailscrollarea.h"
#include "thumbnaillabel.h"

ThumbnailScrollArea::ThumbnailScrollArea(QWidget *parent) :
    QScrollArea(parent),
    scrollStep(150),
    defaultHeight(125)
{
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  //  setFrameShape(QFrame::NoFrame);
    this->setMinimumHeight(defaultHeight);
    this->hide();
    strip = new ThumbnailStrip(this);
    strip->setMinimumHeight(this->minimumHeight());
    this->setWidgetResizable(true);
    this->setWidget(strip);
    connect(strip, SIGNAL(thumbnailClicked(int)),
            this, SIGNAL(thumbnailClicked(int)));
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
    loadVisibleThumbnails();
    event->accept();
}

void ThumbnailScrollArea::loadVisibleThumbnails() {
    for(int i=0; i<strip->thumbnailLabels.count(); i++) {
        if(strip->thumbnailLabels.at(i)->state == EMPTY  &&
            childVisible(strip->thumbnailLabels.at(i)))
        {
            strip->thumbnailLabels.at(i)->state = LOADING;
            emit thumbnailRequested(i);
        }
    }
}

void ThumbnailScrollArea::setThumbnail(int pos, const QPixmap* thumb) {
    strip->thumbnailLabels.at(pos)->setPixmap(*thumb);
    strip->thumbnailLabels.at(pos)->state = LOADED;
}

bool ThumbnailScrollArea::childVisible(ThumbnailLabel *label) {
    //qDebug() << this->widget()->visibleRegion() << "   <>   " << label->relativeRect();
    return this->widget()->visibleRegion().intersects(label->relativeRect());
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

