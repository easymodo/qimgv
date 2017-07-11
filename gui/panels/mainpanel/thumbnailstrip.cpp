#include "thumbnailstrip.h"

ThumbnailStrip::ThumbnailStrip()
    : panelSize(122),
      current(-1),
      thumbnailSize(100),
      thumbnailInterval(0),
      idCounter(0)
{
    thumbnailLabels = new QList<ThumbnailLabel*>();

    scene = new QGraphicsScene(this); // move scene to view class?
    thumbnailFrame.view()->setScene(scene);
    thumbnailFrame.setFrameShape(QFrame::NoFrame);

    // main layout
    layout = new QBoxLayout(QBoxLayout::LeftToRight);
    layout->setSpacing(0);
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(&thumbnailFrame);

    this->setLayout(layout);

    // other extremely important things
    thumbnailFrame.setAccessibleName("thumbnailView");
    loadTimer.setSingleShot(true);

    this->setAttribute(Qt::WA_NoMousePropagation, true);
    this->setFocusPolicy(Qt::NoFocus);

    connect(&thumbnailFrame, SIGNAL(thumbnailClicked(int)),
            this, SLOT(onThumbnailClick(int)));
    connect(&loadTimer, SIGNAL(timeout()), this, SLOT(loadVisibleThumbnails()));
    connect(&thumbnailFrame, SIGNAL(scrolled()), this, SLOT(loadVisibleThumbnails()));
}

void ThumbnailStrip::populate(int count) {
    if(count >= 0 ) {
        for(int i = thumbnailLabels->count() - 1; i >= 0; --i) {
            delete thumbnailLabels->takeAt(0);
        }
        scene->clear();
        //recreate list
        delete thumbnailLabels;
        thumbnailLabels = new QList<ThumbnailLabel*>();

        for(int i = 0; i < count; i++) {
            addThumbnailLabel();
        }
        setThumbnailSize(thumbnailSize);
        thumbnailFrame.view()->resetViewport();
    }
}

void ThumbnailStrip::fillPanel(int count) {
    if(count >= 0 ) {
        current = -1;
        loadTimer.stop();
        populate(count);
        loadVisibleThumbnails();
    }
}

void ThumbnailStrip::addThumbnailLabel() {
    addItemAt(thumbnailLabels->count());
}

void ThumbnailStrip::addItemAt(int pos) {
    lock();
    ThumbnailLabel *thumbLabel = new ThumbnailLabel();
    thumbnailLabels->insert(pos, thumbLabel);
    thumbLabel->setLabelNum(pos);
    scene->addItem(thumbLabel);

    // set the id
    posIdHash.insert(pos, idCounter);
    posIdHashReverse.insert(idCounter, pos);
    idCounter++;

    // set position for new & move existing items right
    updateThumbnailPositions(pos, thumbnailLabels->count() - 1);
    unlock();
}

void ThumbnailStrip::updateThumbnailPositions(int start, int end) {
    if(start > end || !checkRange(start) || !checkRange(end)) {
        //qDebug() << "ThumbnailStrip::updateThumbnailPositions() - arguments out of range";
        return;
    }
    // assume all thumbnails are the same size
    int thumbWidth = thumbnailLabels->at(start)->boundingRect().width() + thumbnailInterval;
    ThumbnailLabel *tmp;
    for(int i = start; i <= end; i++) {
        tmp = thumbnailLabels->at(i);
        tmp->setPos(i * thumbWidth, 0);
    }
}

// fit scene to it's contents size
void ThumbnailStrip::updateSceneSize() {
    scene->setSceneRect(scene->itemsBoundingRect());
}

void ThumbnailStrip::highlightThumbnail(int pos) {
    // this code fires twice on click. fix later
    // also wont highlight new label after removing file
    if(current != pos) {
        ensureThumbnailVisible(pos);
        // disable highlighting on previous thumbnail
        if(checkRange(current)) {
            thumbnailLabels->at(current)->setHighlighted(false, false);
        }
        // highlight the new one
        if(checkRange(pos)) {
            thumbnailLabels->at(pos)->setHighlighted(true, false);
            current = pos;
        }
    }
    loadVisibleThumbnails();
}

void ThumbnailStrip::ensureThumbnailVisible(int pos) {
    if(checkRange(pos))
        thumbnailFrame.view()->ensureVisible(thumbnailLabels->at(pos)->sceneBoundingRect(),
                                             thumbnailSize / 2,
                                             0);
}

void ThumbnailStrip::onThumbnailClick(int pos) {
    highlightThumbnail(pos);
    emit thumbnailClicked(pos);
}

void ThumbnailStrip::loadVisibleThumbnailsDelayed() {
    loadTimer.stop();
    loadTimer.start(LOAD_DELAY);
}

void ThumbnailStrip::loadVisibleThumbnails() {
    loadTimer.stop();
    if(isVisible()) {
        QRectF visibleRect = thumbnailFrame.view()->mapToScene(
                    thumbnailFrame.view()->viewport()->geometry()).boundingRect();
        // grow rectangle to cover nearby offscreen items
        visibleRect.adjust(-OFFSCREEN_PRELOAD_AREA, 0, OFFSCREEN_PRELOAD_AREA, 0);
        QList<QGraphicsItem *>visibleItems = scene->items(visibleRect,
                                                   Qt::IntersectsItemShape,
                                                   Qt::AscendingOrder);
        QList<int> loadList;
        for(int i = 0; i < visibleItems.count(); i++) {
            ThumbnailLabel* label = qgraphicsitem_cast<ThumbnailLabel*>(visibleItems.at(i));
            if(label->state == EMPTY) {
                loadList.append(label->labelNum());
            }
        }
        if(loadList.count()) {
            emit thumbnailRequested(loadList, thumbnailSize);
        }
    }
}

bool ThumbnailStrip::checkRange(int pos) {
    if(pos >= 0 && pos < thumbnailLabels->count())
        return true;
    else
        return false;
}

void ThumbnailStrip::lock() {
    mutex.lock();
}

void ThumbnailStrip::unlock() {
    mutex.unlock();
}

// called by loader when thubmnail is ready
void ThumbnailStrip::setThumbnail(int pos, Thumbnail *thumb) {
    lock();
    if(thumb && thumb->size == thumbnailSize && checkRange(pos)) {
        thumbnailLabels->at(pos)->setThumbnail(thumb);
        thumbnailLabels->at(pos)->state = LOADED;
        /*
        if(pos != current) {
            thumbnailLabels->at(pos)->setOpacityAnimated(OPACITY_INACTIVE, ANIMATION_SPEED_NORMAL);
        }
        */
    } else {
        // dispose of thumbnail if it is unneeded
        delete thumb;
    }
    unlock();
}

void ThumbnailStrip::setThumbnailSize(int newSize) {
    if(newSize >= 20) {
        thumbnailSize = newSize;
        for(int i=0; i<thumbnailLabels->count(); i++) {
            thumbnailLabels->at(i)->setThumbnailSize(newSize);
        }
        updateThumbnailPositions(0, thumbnailLabels->count() - 1);
        updateSceneSize();
        ensureThumbnailVisible(current);
    }
}

void ThumbnailStrip::removeItemAt(int pos) {
    lock();
    if(checkRange(pos)) {
        if(pos == current)
            current = -1;
        ThumbnailLabel *thumb = thumbnailLabels->takeAt(pos);
        scene->removeItem(thumb);
        // move items left
        ThumbnailLabel *tmp;
        for(int i = pos; i < thumbnailLabels->count(); i++) {
            tmp = thumbnailLabels->at(i);
            tmp->moveBy(-tmp->boundingRect().width(), 0);
            tmp->setLabelNum(i);
        }
        updateSceneSize();
    }
    unlock();
}

// resizes thumbnailSize to fit new widget size
// TODO: find some way to make this trigger while hidden
void ThumbnailStrip::resizeEvent(QResizeEvent *event) {
    Q_UNUSED(event)
    QWidget::resizeEvent(event);
    if(event->oldSize().height() != height())
        updateThumbnailSize();
    if(event->oldSize().width() < width())
        loadVisibleThumbnailsDelayed();
}

void ThumbnailStrip::showEvent(QShowEvent *event) {
    QWidget::showEvent(event);
    loadVisibleThumbnails();
}

// update size based on widget's size
// reposition thumbnails within scene if needed
void ThumbnailStrip::updateThumbnailSize() {
    int newSize = this->height() - 24;
    if( newSize % 2 )
        --newSize;
    if(newSize != thumbnailSize) {
        setThumbnailSize(newSize);
    }
}

ThumbnailStrip::~ThumbnailStrip() {
}
