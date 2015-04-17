#include "thumbnailstrip.h"

ThumbnailStrip::ThumbnailStrip(const ImageCache *_cache, QWidget *parent) : QGraphicsView(parent),
    cache(_cache)
{

    scene = new CustomScene;

    QPen outlinePen(Qt::black);
    outlinePen.setWidth(2);

    this->setScene(scene);
    this->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    populate();
    scene->setSceneRect(rect());
    connect(scene, SIGNAL(click(int)), this, SLOT(sceneClicked(int)));
    this->show();
}

void ThumbnailStrip::populate() {
   // scene->clear();

    for(int i=0; i<cache->length(); i++) {
        addItem(i);
    }
}

void ThumbnailStrip::addItem(int pos) {
    QGraphicsPixmapItem *item = scene->addPixmap(QPixmap::fromImage(*cache->thumbnailAt(pos)));
//    qDebug() << pos;
    item->setPos(pos*100,0);
//    qDebug() << this->sceneRect();
    this->resize(sceneRect().size().toSize());
    //qDebug() << rect();
}

void ThumbnailStrip::sceneClicked(int wPos) {
    int clicked = wPos/100;
    qDebug() << clicked;
    emit thumbnailClicked(clicked);

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
