#include "thumbnailstrip.h"

ThumbnailStrip::ThumbnailStrip(QWidget *_parent) : QWidget(_parent),
    parent(_parent)
{
    layout = new QHBoxLayout(this);
    layout->setAlignment(Qt::AlignLeft);
    //layout->setSizeConstraint(QLayout::SetFixedSize);
   //this->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
   // this->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
   // layout->setDirection(QBoxLayout::LeftToRight);
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
    // qt's layouts are the most retarded thing
    layout->activate();
}

QSize ThumbnailStrip::sizeHint() const {
    return QSize(300, 50);
}

void ThumbnailStrip::addItem(int pos) {
    ThumbnailLabel *thumbLabel = new ThumbnailLabel(this);

    layout->addWidget(thumbLabel);
    thumbnailLabels.append(thumbLabel);
    connect(thumbLabel, SIGNAL(clicked(ThumbnailLabel*)),
            this, SLOT(slotThumbnailClicked(ThumbnailLabel*)));
    this->setGeometry(0,0, thumbnailLabels.count()*(thumbLabel->height()+layout->spacing()), this->height());
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
