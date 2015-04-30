#include "thumbnailstrip.h"

ThumbnailStrip::ThumbnailStrip(QWidget *_parent) : QWidget(_parent),
    parent(_parent)
{
    layout = new QHBoxLayout(this);
    //layout->setSizeConstraint(QLayout::SetFixedSize);
    this->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
    layout->setDirection(QBoxLayout::LeftToRight);
    this->show();
}

void ThumbnailStrip::populate(int count) {
    thumbnailLabels.clear();
    while(layout->count() > 0) {
        QLayoutItem *item = layout->takeAt(0);
        delete item->widget();
        delete item;
    }
    for(int i=0; i<count; i++) {
        addItem(i);
    }
}

void ThumbnailStrip::addItem(int pos) {
    ThumbnailLabel *thumbLabel = new ThumbnailLabel();
    thumbLabel->setGeometry(0,0,100,100);

    layout->addWidget(thumbLabel);
    thumbnailLabels.append(thumbLabel);
    connect(thumbLabel, SIGNAL(clicked(ThumbnailLabel*)),
            this, SLOT(slotThumbnailClicked(ThumbnailLabel*)));
}

void ThumbnailStrip::wheelEvent(QWheelEvent *event) {

    event->ignore();
}

void ThumbnailStrip::slotThumbnailClicked(ThumbnailLabel* label) {
    int id = thumbnailLabels.indexOf(label);
    if(id!=-1) {
        emit thumbnailClicked(id);
    }
}

ThumbnailStrip::~ThumbnailStrip() {

}
