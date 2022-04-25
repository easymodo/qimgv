#include "thumbnailstrip.h"

// TODO: move item highlight logic to base class

ThumbnailStrip::ThumbnailStrip(QWidget *parent) : ThumbnailView(THUMBNAILVIEW_HORIZONTAL, parent) {
    this->setAttribute(Qt::WA_NoMousePropagation, true);
    this->setFocusPolicy(Qt::NoFocus);
    setupLayout();
    readSettings();
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
    ThumbnailWidget *widget = new ThumbnailWidget();
    widget->setPadding(thumbPadding);
    widget->setMargins(thumbMarginX, thumbMarginY);
    widget->setThumbStyle(mCurrentStyle);
    widget->setThumbnailSize(mThumbnailSize);
    return widget;
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
    int thumbWidth = static_cast<int>(thumbnails.at(start)->boundingRect().width());
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

void ThumbnailStrip::focusOnSelection() {
    if(selection().isEmpty())
        return;
    focusOn(selection().last());
}

void ThumbnailStrip::readSettings() {
    thumbMarginX = 2;
    thumbMarginY = 4;
    thumbPadding = 9;
    if(settings->thumbPanelStyle() == TH_PANEL_SIMPLE)
        mCurrentStyle = THUMB_SIMPLE;
    else
        mCurrentStyle = THUMB_NORMAL_CENTERED;
    mThumbnailSize = qBound(20, settings->panelPreviewsSize(), 300);
    // apply style, size & reposition
    for(int i = 0; i < thumbnails.count(); i++) {
        thumbnails.at(i)->setPadding(thumbPadding);
        thumbnails.at(i)->setMargins(thumbMarginX, thumbMarginY);
        thumbnails.at(i)->setThumbStyle(mCurrentStyle);
        thumbnails.at(i)->setThumbnailSize(mThumbnailSize);
    }
    updateThumbnailPositions(0, thumbnails.count() - 1);
    fitSceneToContents();
    ensureThumbnailVisible(lastSelected());
    setCropThumbnails(settings->squareThumbnails());
}

void ThumbnailStrip::ensureThumbnailVisible(int pos) {
    if(checkRange(pos))
        ensureVisible(thumbnails.at(pos)->sceneBoundingRect(),
                      mThumbnailSize / 2, 0);
}

QSize ThumbnailStrip::itemSize() {
    if(!thumbnails.count()) {
        ThumbnailWidget w;
        w.setPadding(thumbPadding);
        w.setMargins(thumbMarginX, thumbMarginY);
        w.setThumbStyle(mCurrentStyle);
        w.setThumbnailSize(mThumbnailSize);
        return w.boundingRect().size().toSize();
    } else {
        return thumbnails.at(0)->boundingRect().size().toSize();
    }
}

void ThumbnailStrip::resizeEvent(QResizeEvent *event) {
    ThumbnailView::resizeEvent(event);
    fitSceneToContents();
    if(event->oldSize().width() < width())
        loadVisibleThumbnailsDelayed();
}
