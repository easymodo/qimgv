#include "thumbnailstrip.h"

ThumbnailStrip::ThumbnailStrip(ImageCache *_cache, QWidget *parent) : QWidget(parent)
{
    cache = _cache;
    layout = new QHBoxLayout(this);
    layout->setSizeConstraint(QLayout::SetMinAndMaxSize);
    qDebug() << this->rect();
    connect(cache, SIGNAL(initialized()), this, SLOT(populate()));
    this->show();
}

void ThumbnailStrip::populate() {
    thumbnailLabels.clear();
    for(int i=0; i<cache->length(); i++) {
        addItem(i);
    }
}

void ThumbnailStrip::addItem(int pos) {
    ThumbnailLabel *thumbLabel = new ThumbnailLabel();
    thumbLabel->setPixmap(QPixmap::fromImage(*cache->thumbnailAt(pos)));
    layout->addWidget(thumbLabel);
    thumbnailLabels.push_back(thumbLabel);
    connect(thumbLabel, SIGNAL(clicked(QLabel*)),
            this, SLOT(slotThumbnailClicked(QLabel*)));
}

void ThumbnailStrip::wheelEvent(QWheelEvent *event) {
    event->ignore();
}

void ThumbnailStrip::slotThumbnailClicked(QLabel* label) {
    emit thumbnailClicked(thumbnailLabels.indexOf(label));
}

ThumbnailStrip::~ThumbnailStrip() {

}
