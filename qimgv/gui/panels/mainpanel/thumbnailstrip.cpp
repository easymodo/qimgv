#include "thumbnailstrip.h"

// TODO: move item highlight logic to base class

ThumbnailStrip::ThumbnailStrip(QWidget *parent)
    : ThumbnailView(THUMBNAILVIEW_HORIZONTAL, parent),
      panelSize(100),
      thumbnailSpacing(0)
{
    this->setAttribute(Qt::WA_NoMousePropagation, true);
    this->setFocusPolicy(Qt::NoFocus);
    setupLayout();

    readSettings();
    connect(settings, SIGNAL(settingsChanged()), this, SLOT(readSettings()));
}

void ThumbnailStrip::updateScrollbarIndicator() {
    if(!thumbnails.count() || lastSelected() == -1)
        return;
    qreal itemCenter = (qreal)(lastSelected() + 0.5) / itemCount();
    if(scrollBar->orientation() == Qt::Horizontal)
        indicator = QRect(scrollBar->width() * itemCenter - indicatorSize, 2, indicatorSize, scrollBar->height() - 4);
    else
        indicator = QRect(2, scrollBar->height() * itemCenter - indicatorSize, scrollBar->width() - 4, indicatorSize);
}

//  no layout; manual item positioning
//  graphical issues otherwise
void ThumbnailStrip::setupLayout() {
    this->setAlignment(Qt::AlignLeft | Qt::AlignTop);
}

ThumbnailWidget* ThumbnailStrip::createThumbnailWidget() {
    ThumbnailWidget *widget = new ThumbnailGridWidget();
    widget->setDrawLabel(false);
    widget->setPadding(6);
    widget->setThumbnailSize(this->mThumbnailSize);
    return widget;
}

void ThumbnailStrip::ensureSelectedItemVisible() {

}

void ThumbnailStrip::addItemToLayout(ThumbnailWidget* widget, int pos) {
    scene.addItem(widget);
    updateThumbnailPositions(pos, thumbnails.count() - 1);
}

void ThumbnailStrip::removeItemFromLayout(int pos) {
    if(checkRange(pos)) {
        ThumbnailWidget *thumb = thumbnails.at(pos);
        scene.removeItem(thumb);
        // move items left
        ThumbnailWidget *tmp;
        for(int i = pos; i < thumbnails.count(); i++) {
            tmp = thumbnails.at(i);
            tmp->moveBy(-tmp->boundingRect().width(), 0);
        }
    }
}

void ThumbnailStrip::removeAll() {
    scene.clear(); // also calls delete on all items
    thumbnails.clear();
}

void ThumbnailStrip::updateThumbnailPositions() {
    updateThumbnailPositions(0, thumbnails.count() - 1);
}

void ThumbnailStrip::updateThumbnailPositions(int start, int end) {
    if(start > end || !checkRange(start) || !checkRange(end)) {
        return;
    }
    // assume all thumbnails are the same size
    int thumbWidth = static_cast<int>(thumbnails.at(start)->boundingRect().width()) + thumbnailSpacing;
    ThumbnailWidget *tmp;
    for(int i = start; i <= end; i++) {
        tmp = thumbnails.at(i);
        tmp->setPos(i * thumbWidth, 0);
    }
}

void ThumbnailStrip::focusOn(int index) {
    if(!checkRange(index))
        return;
    ThumbnailWidget *thumb = thumbnails.at(index);
    ensureVisible(thumb, 0, 0);
    loadVisibleThumbnails();
}

void ThumbnailStrip::readSettings() {
    setCropThumbnails(settings->squareThumbnails());
}

void ThumbnailStrip::ensureThumbnailVisible(int pos) {
    if(checkRange(pos))
        ensureVisible(thumbnails.at(pos)->sceneBoundingRect(),
                      mThumbnailSize / 2, 0);
}

// scene stuff??
void ThumbnailStrip::setThumbnailSize(int newSize) {
    if(newSize >= 20) {
        mThumbnailSize = newSize;
        for(int i=0; i<thumbnails.count(); i++) {
            thumbnails.at(i)->setThumbnailSize(newSize);
        }
        //scene.invalidate(scene.sceneRect());
        updateThumbnailPositions(0, thumbnails.count() - 1);
        fitSceneToContents();
        ensureThumbnailVisible(lastSelected());
    }
}

// resizes thumbnailSize to fit new widget size
// TODO: find some way to make this trigger while hidden
void ThumbnailStrip::resizeEvent(QResizeEvent *event) {
    ThumbnailView::resizeEvent(event);
    fitSceneToContents();
    if(event->oldSize().height() != height())
        updateThumbnailSize();
    if(event->oldSize().width() < width())
        loadVisibleThumbnailsDelayed();
}

// update size based on widget's size
// reposition thumbnails within scene if needed
void ThumbnailStrip::updateThumbnailSize() {
    int newSize = height() - 30;
    if( newSize % 2 )
        --newSize;
    if(newSize != mThumbnailSize) {
        setThumbnailSize(newSize);
    }
}
