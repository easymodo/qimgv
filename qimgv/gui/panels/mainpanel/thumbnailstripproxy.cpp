#include "thumbnailstripproxy.h"

ThumbnailStripProxy::ThumbnailStripProxy(QWidget *parent)
    : QWidget(parent)
{
    layout.setContentsMargins(0,0,0,0);
}

void ThumbnailStripProxy::init() {
    if(thumbnailStrip)
        return;
    thumbnailStrip.reset(new ThumbnailStrip());
    thumbnailStrip->setParent(this);
    layout.addWidget(thumbnailStrip.get());
    this->setFocusProxy(thumbnailStrip.get());
    this->setLayout(&layout);

    connect(thumbnailStrip.get(), &ThumbnailStrip::itemActivated, this, &ThumbnailStripProxy::itemActivated);
    connect(thumbnailStrip.get(), &ThumbnailStrip::thumbnailsRequested, this, &ThumbnailStripProxy::thumbnailsRequested);

    thumbnailStrip->show();

    // apply buffer
    thumbnailStrip->populate(stateBuf.itemCount);
    thumbnailStrip->selectIndex(stateBuf.selectedIndex);
    // wait till layout stuff happens
    // before calling focusOn()
    qApp->processEvents();
    thumbnailStrip->focusOn(stateBuf.selectedIndex);
}

void ThumbnailStripProxy::populate(int count) {
    if(thumbnailStrip) {
        thumbnailStrip->populate(count);
    } else {
        stateBuf.itemCount = count;
    }
}

void ThumbnailStripProxy::setThumbnail(int pos, std::shared_ptr<Thumbnail> thumb) {
    if(thumbnailStrip) {
        thumbnailStrip->setThumbnail(pos, thumb);
    }
}

void ThumbnailStripProxy::selectIndex(int index) {
    if(thumbnailStrip) {
        thumbnailStrip->selectIndex(index);
    } else {
        stateBuf.selectedIndex = index;
    }
}

int ThumbnailStripProxy::selectedIndex() {
    if(thumbnailStrip) {
        return thumbnailStrip->selectedIndex();
    } else {
        return stateBuf.selectedIndex;
    }
}

void ThumbnailStripProxy::focusOn(int index) {
    if(thumbnailStrip) {
        thumbnailStrip->focusOn(index);
    }
}

void ThumbnailStripProxy::insertItem(int index) {
    if(thumbnailStrip) {
        thumbnailStrip->insertItem(index);
    } else {
        stateBuf.itemCount++;
    }
}

void ThumbnailStripProxy::removeItem(int index) {
    if(thumbnailStrip) {
        thumbnailStrip->removeItem(index);
    } else {
        stateBuf.itemCount--;
        if(index < stateBuf.selectedIndex) {
            stateBuf.selectedIndex--;
        } else if(index == stateBuf.selectedIndex) {
            if(stateBuf.selectedIndex >= stateBuf.itemCount)
                stateBuf.selectedIndex = stateBuf.itemCount - 1;
            else
                stateBuf.selectedIndex = index;
        }
    }
}

void ThumbnailStripProxy::reloadItem(int index) {
    if(thumbnailStrip)
        thumbnailStrip->reloadItem(index);
}

void ThumbnailStripProxy::setDirectoryPath(QString path) {
    Q_UNUSED(path)
}

void ThumbnailStripProxy::addItem() {
    if(thumbnailStrip) {
        thumbnailStrip->addItem();
    } else {
        stateBuf.itemCount++;
    }
}

void ThumbnailStripProxy::showEvent(QShowEvent *event) {
    init();
    QWidget::showEvent(event);
}
