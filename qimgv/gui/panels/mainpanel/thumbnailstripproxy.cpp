#include "thumbnailstripproxy.h"

ThumbnailStripProxy::ThumbnailStripProxy(QWidget *parent)
    : QWidget(parent)
{
    layout.setContentsMargins(0,0,0,0);
}

void ThumbnailStripProxy::init() {
    if(thumbnailStrip)
        return;
    qApp->processEvents(); // chew through events in case we have something that alters stateBuf in queue
    QMutexLocker ml(&m);
    thumbnailStrip.reset(new ThumbnailStrip());
    thumbnailStrip->setParent(this);
    ml.unlock();
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
    thumbnailStrip->focusOnSelection();
}

bool ThumbnailStripProxy::isInitialized() {
    return (thumbnailStrip != nullptr);
}

void ThumbnailStripProxy::populate(int count) {
    QMutexLocker ml(&m);
    stateBuf.itemCount = count;
    if(thumbnailStrip) {
        ml.unlock();
        thumbnailStrip->populate(stateBuf.itemCount);
    } else {
        stateBuf.selection.clear();
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

void ThumbnailStripProxy::focusOnSelection() {
    if(thumbnailStrip) {
        thumbnailStrip->focusOnSelection();
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

void ThumbnailStripProxy::setDragHover(int index) {
    if(thumbnailStrip)
        thumbnailStrip->setDragHover(index);
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

QSize ThumbnailStripProxy::itemSize() {
    return thumbnailStrip->itemSize();
}

void ThumbnailStripProxy::readSettings() {
    if(thumbnailStrip)
        thumbnailStrip->readSettings();
}

void ThumbnailStripProxy::showEvent(QShowEvent *event) {
    init();
    QWidget::showEvent(event);
}
