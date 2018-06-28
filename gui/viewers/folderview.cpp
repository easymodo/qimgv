#include "folderview.h"

FolderView::FolderView(QWidget *parent) : QGraphicsView(parent) {
    this->setScene(&scene);
    flowLayout = new FlowLayout();
    //flowLayout = new QGraphicsLinearLayout();
    //layout = new QVBoxLayout(this);
    scene.addItem(&holderWidget);

    populate(40);

    this->setFixedWidth(parent->width());
    holderWidget.setMaximumWidth(this->width());

    holderWidget.setLayout(flowLayout);
}

void FolderView::populate(int count) {
    if(count >= 0 ) {
        for(int i = 0; i < thumbnails.count() - 1; i++) {
            ThumbnailLabel *tmp = thumbnails.takeAt(0);
            flowLayout->removeAt(0);
            delete tmp;
        }
        for(int i = 0; i < count; i++) {
            ThumbnailLabel *label = new ThumbnailLabel();
            label->setLabelNum(count);
            label->setThumbnailSize(300);
            //label->setGeometry(QRectF(0,0,300,300));
            thumbnails.append(label);
            flowLayout->addItem(label);
        }
    }

    //qDebug() << holderWidget.size() << scene.sceneRect();
}

void FolderView::setThumbnail(int pos, Thumbnail *thumb) {
    if(thumb /*&& thumb->size() == floor(thumbnailSize*qApp->devicePixelRatio()) && checkRange(pos) */) {
       // qDebug() ->pos()<< "SET: " << pos;
        thumbnails.at(pos)->setThumbnail(thumb);
        thumbnails.at(pos)->state = LOADED;
    } else {
        // dispose of thumbnail if it is unneeded
        //delete thumb;
    }
}

void FolderView::show() {
    QGraphicsView::show();
    qDebug() << "widget size: " << holderWidget.size() << thumbnails.at(1)->geometry() << thumbnails.at(1)->boundingRect();
}

FolderView::~FolderView() {
    delete layout;
}
