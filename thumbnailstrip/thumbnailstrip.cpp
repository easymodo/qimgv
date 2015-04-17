#include "thumbnailstrip.h"

ThumbnailStrip::ThumbnailStrip(ImageCache *_cache, QWidget *parent) : QGraphicsView(parent)
{
    scene = new CustomScene;
    cache = _cache;
    QPen outlinePen(Qt::black);
    outlinePen.setWidth(2);

    this->setScene(scene);
    this->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scene->setSceneRect(rect());
    connect(scene, SIGNAL(click(int)), this, SLOT(sceneClicked(int)));
    connect(cache, SIGNAL(initialized()), this, SLOT(populate()));
    this->show();
}

void ThumbnailStrip::populate() {
    scene->clear();
    itemCount = 0;
    scene->setSceneRect(rect());

    for(int i=0; i<cache->length(); i++) {
        addItem(i);
    }
}

void ThumbnailStrip::addItem(int pos) {
    QGraphicsPixmapItem *item = scene->addPixmap(QPixmap::fromImage(*cache->thumbnailAt(pos)));
    item->setPos(pos*100,0);
    this->resize(sceneRect().size().toSize());
    itemCount++;
}

void ThumbnailStrip::sceneClicked(int wPos) {
    int clicked = wPos/100;
    if(clicked < itemCount) {
        emit thumbnailClicked(clicked);
    }

}

void ThumbnailStrip::wheelEvent(QWheelEvent *event) {
    int scrollAmount = 200; //px
    event->setAccepted(true);
    QPointF viewCenter = mapToScene(width() / 2, 0);

    if(event->angleDelta().ry() < 0) {
        viewCenter += QPointF(scrollAmount, 0);
    }
    else {
        viewCenter -= QPointF(scrollAmount, 0);
    }
    centerOn(viewCenter);
}

ThumbnailStrip::~ThumbnailStrip() {

}
