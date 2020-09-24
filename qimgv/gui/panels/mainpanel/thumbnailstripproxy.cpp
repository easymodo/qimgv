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
    thumbnailStrip->select(stateBuf.selection);
    // wait till layout stuff happens
    // before calling focusOn()
    qApp->processEvents();
    thumbnailStrip->focusOn(stateBuf.selection.first());
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

void ThumbnailStripProxy::select(QList<int> indices) {
    if(thumbnailStrip) {
        thumbnailStrip->select(indices);
    } else {
        stateBuf.selection = indices;
    }
}

void ThumbnailStripProxy::select(int index) {
    if(thumbnailStrip) {
        thumbnailStrip->select(index);
    } else {
        stateBuf.selection.clear();
        stateBuf.selection << index;
    }
}

QList<int> ThumbnailStripProxy::selection() {
    if(thumbnailStrip) {
        return thumbnailStrip->selection();
    } else {
        return stateBuf.selection;
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
        stateBuf.selection.removeAll(index);
        for(int i=0; i < stateBuf.selection.count(); i++) {
            if(stateBuf.selection[i] > index)
                stateBuf.selection[i]--;
        }
        if(!stateBuf.selection.count())
            stateBuf.selection << ((index >= stateBuf.itemCount) ? stateBuf.itemCount - 1 : index);
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
