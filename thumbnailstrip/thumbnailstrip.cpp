#include "thumbnailstrip.h"

ThumbnailStrip::ThumbnailStrip(QWidget *parent)
    : QGraphicsView(parent),
      scrollStep(200),
      defaultHeight(132),
      loadDelay(120)
{
    widget = new QGraphicsWidget();
    scene = new CustomScene;
    layout = new QGraphicsLinearLayout(Qt::Horizontal);
    widget->setLayout(layout);
    scene->addItem(widget);

    this->setScene(scene);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->show();
    this->setGeometry(0,0,500,150);

    loadTimer.setSingleShot(true);

    connect(scene, SIGNAL(sceneClick(QPointF)),
            this, SLOT(sceneClicked(QPointF)));

    connect(this->horizontalScrollBar(), SIGNAL(valueChanged(int)),
            this, SLOT(loadVisibleThumbnailsDelayed()));
    connect(this->horizontalScrollBar(), SIGNAL(sliderMoved(int)),
            this, SLOT(loadVisibleThumbnailsDelayed()));
    connect(&loadTimer, SIGNAL(timeout()), this, SLOT(loadVisibleThumbnails()));
}

void ThumbnailStrip::fillPanel(int count) {
    previous = -1;
    loadTimer.stop();
    this->horizontalScrollBar()->setValue(0);
    populate(count);
    loadVisibleThumbnails();
}

void ThumbnailStrip::selectThumbnail(int pos) {
    if(previous != -1) {
        thumbnailLabels.at(previous)->setHighlighted(false);
    }
    thumbnailLabels.at(pos)->setHighlighted(true);
    previous = pos;
    if(!childVisibleEntirely(pos)) {
        centerOn(thumbnailLabels.at(pos)->scenePos());
    }

}

void ThumbnailStrip::populate(int count) {
    // clear thumb list
    // this will fail if list items != layout items
    // shouldnt happen though
    for(int i = layout->count()-1; i >= 0; --i) {
        layout->removeAt(0);
        delete thumbnailLabels.takeAt(0);
    }
    thumbnailLabels.clear();

    //add items
    for(int i=0; i<count; i++) {
        addItem();
    }

    layout->invalidate();
    layout->activate();
    scene->setSceneRect(scene->itemsBoundingRect());
}

// in theory faster than scene's version
QRectF ThumbnailStrip::itemsBoundingRect() {
    QRectF boundingRect(0,0,0,0);
    if(!thumbnailLabels.isEmpty()) {
        boundingRect.setTopLeft(
                    thumbnailLabels.at(0)->sceneBoundingRect().topLeft());
        boundingRect.setBottomRight(
                    thumbnailLabels.at(thumbnailLabels.count()-1)->sceneBoundingRect().bottomRight());
    }
    qDebug() << boundingRect;
    return boundingRect;
}


void ThumbnailStrip::loadVisibleThumbnailsDelayed() {
    loadTimer.stop();
    loadTimer.start(loadDelay);
}


void ThumbnailStrip::loadVisibleThumbnails() {
    loadTimer.stop();

    updateVisibleRegion();

    int counter = 0;
    for(int i=0; i<thumbnailLabels.count(); i++) {
        if(thumbnailLabels.at(i)->state == EMPTY  && childVisible(i))
        {
            thumbnailLabels.at(i)->state = LOADING;
            counter++;
            emit thumbnailRequested(i);
        }
    }
}

void ThumbnailStrip::addItem() {
    ThumbnailLabel *thumbLabel = new ThumbnailLabel();
    thumbnailLabels.append(thumbLabel);
    layout->addItem(thumbLabel);
}

void ThumbnailStrip::setThumbnail(int pos, const QPixmap* thumb) {
    thumbnailLabels.at(pos)->setPixmap(*thumb);
    thumbnailLabels.at(pos)->state = LOADED;
}

void ThumbnailStrip::updateVisibleRegion() {
    QRect viewport_rect(0, 0, width(), height());
    visibleRegion = mapToScene(viewport_rect).boundingRect();
    visibleRegion.adjust(-350,0,350,0);
}

bool ThumbnailStrip::childVisible(int pos) {
    if(thumbnailLabels.count() >pos) {
        return thumbnailLabels.at(pos)->
                sceneBoundingRect().intersects(visibleRegion);
    } else {
        return false;
    }
}

bool ThumbnailStrip::childVisibleEntirely(int pos) {
    if(thumbnailLabels.count() >pos) {
        QRectF visibleRegionReduced = visibleRegion.adjusted(500,0,-500,0);
        return thumbnailLabels.at(pos)->
                sceneBoundingRect().intersects(visibleRegionReduced);
    } else {
        return false;
    }
}

void ThumbnailStrip::sceneClicked(QPointF pos) {
    ThumbnailLabel *item = (ThumbnailLabel*)scene->itemAt(pos, QGraphicsView::transform());
    int itemPos = thumbnailLabels.indexOf(item);
    if(itemPos!=-1) {
        emit thumbnailClicked(itemPos);
    }
}

void ThumbnailStrip::wheelEvent(QWheelEvent *event) {
    event->setAccepted(true);
    QPointF viewCenter = mapToScene(width() / 2, 0);

    if(event->angleDelta().ry() < 0) {
        viewCenter += QPointF(scrollStep, 0);
    }
    else {
        viewCenter -= QPointF(scrollStep, 0);
    }
    centerOn(viewCenter);
}

void ThumbnailStrip::parentResized(QSize parentSize) {
    this->setGeometry( QRect(0, parentSize.height() - defaultHeight + 1,
                             parentSize.width(), defaultHeight) );
    loadVisibleThumbnailsDelayed();
}

void ThumbnailStrip::leaveEvent(QEvent *event) {
    Q_UNUSED(event)
    this->hide();
}

ThumbnailStrip::~ThumbnailStrip() {

}
