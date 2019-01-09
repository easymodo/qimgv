#include "thumbnailstrip.h"

// TODO: move item highlight logic to base class

ThumbnailStrip::ThumbnailStrip(QWidget *parent)
    : ThumbnailView(THUMBNAILVIEW_HORIZONTAL, parent),
      panelSize(100),
      current(-1),
      thumbnailSpacing(0)
{
    this->setAttribute(Qt::WA_NoMousePropagation, true);
    this->setFocusPolicy(Qt::NoFocus);
    setupLayout();
}

//  no layout; manual item positioning
//  graphical issues otherwise
void ThumbnailStrip::setupLayout() {
    this->setAlignment(Qt::AlignLeft | Qt::AlignTop);
}

ThumbnailWidget* ThumbnailStrip::createThumbnailWidget() {
    ThumbnailWidget *widget = new ThumbnailWidget();
    widget->setDrawLabel(true);
    return widget;
}

void ThumbnailStrip::ensureSelectedItemVisible() {

}

void ThumbnailStrip::addItemToLayout(ThumbnailWidget* widget, int pos) {
    if(!checkRange(pos))
        return;

    if(pos == current)
        current++;
    scene.addItem(widget);
    updateThumbnailPositions(pos, thumbnails.count() - 1);
}

void ThumbnailStrip::removeItemFromLayout(int pos) {
    if(checkRange(pos)) {
        if(pos == current)
            current = -1;
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

void ThumbnailStrip::highlightThumbnail(int pos) {
    // this code fires twice on click. fix later
    // also wont highlight new label after removing file
    if(current != pos) {
        ensureThumbnailVisible(pos);
        // disable highlighting on previous thumbnail
        if(checkRange(current)) {
            thumbnails.at(current)->setHighlighted(false, false);
        }
        // highlight the new one
        if(checkRange(pos)) {
            thumbnails.at(pos)->setHighlighted(true, false);
            current = pos;
        }
    }
    loadVisibleThumbnails();
}

void ThumbnailStrip::ensureThumbnailVisible(int pos) {
    if(checkRange(pos))
        ensureVisible(thumbnails.at(pos)->sceneBoundingRect(),
                      thumbnailSize / 2,
                      0);
}

// scene stuff??
void ThumbnailStrip::setThumbnailSize(int newSize) {
    if(newSize >= 20) {
        thumbnailSize = newSize;
        for(int i=0; i<thumbnails.count(); i++) {
            thumbnails.at(i)->setThumbnailSize(newSize);
        }
        //scene.invalidate(scene.sceneRect());
        updateThumbnailPositions(0, thumbnails.count() - 1);
        fitSceneToContents();
        ensureThumbnailVisible(current);
    }
}

//  reimplement is base
/*
void ThumbnailStrip::removeItemAt(int pos) {
    lock();
    if(checkRange(pos)) {
        if(pos == current)
            current = -1;
        ThumbnailLabel *thumb = thumbnails.takeAt(pos);
        scene.removeItem(thumb);
        // move items left
        ThumbnailLabel *tmp;
        for(int i = pos; i < thumbnails.count(); i++) {
            tmp = thumbnails.at(i);
            tmp->moveBy(-tmp->boundingRect().width(), 0);
            tmp->setLabelNum(i);
        }
        updateSceneRect();
    }
    unlock();
}
*/

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

// update size based on widget's size
// reposition thumbnails within scene if needed
void ThumbnailStrip::updateThumbnailSize() {
    int newSize = height() - 25;
    if( newSize % 2 )
        --newSize;
    if(newSize != thumbnailSize) {
        setThumbnailSize(newSize);
    }
}
