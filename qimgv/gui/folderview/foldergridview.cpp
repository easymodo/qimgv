#include "foldergridview.h"

// TODO: create a base class for this and the one on panel

FolderGridView::FolderGridView(QWidget *parent)
    : ThumbnailView(THUMBNAILVIEW_VERTICAL, parent),
      shiftedIndex(-1),
      mShowLabels(false)
{
    offscreenPreloadArea = 2300;
    this->viewport()->setAttribute(Qt::WA_OpaquePaintEvent, true);
    this->scene.setBackgroundBrush(QColor(47,47,48)); //#2f2f30 TODO: use qss??

    // turn this off until [multi]selection is implemented
    setDrawScrollbarIndicator(false);

    setupLayout();
    reservedShortcuts << "Up"
                      << "Down"
                      << "Left"
                      << "Right"
                      << "pageUp"
                      << "pageDown"
                      << "Return"
                      << "home"
                      << "end"
                      << "delete";
}

void FolderGridView::updateScrollbarIndicator() {
    if(selectedIndex() < 0)
        return;
    ThumbnailWidget *thumb = thumbnails.at(selectedIndex());
    qreal itemCenter = (thumb->pos().y() + (thumb->height() / 2)) / scene.height();
    indicator = QRect(2, scrollBar->height() * itemCenter - indicatorSize, scrollBar->width() - 4, indicatorSize);
}

// probably unneeded
void FolderGridView::show() {
    ThumbnailView::show();
    setFocus();
}

// probably unneeded
void FolderGridView::hide() {
    ThumbnailView::hide();
    clearFocus();
}

void FolderGridView::setShowLabels(bool mode) {
    mShowLabels = mode;
    for(int i = 0; i < thumbnails.count(); i++) {
        thumbnails.at(i)->setDrawLabel(mShowLabels);
    }
    updateLayout();
    fitToContents();
    ensureSelectedItemVisible();
    emit showLabelsChanged(mShowLabels);
}

void FolderGridView::ensureSelectedItemVisible() {
    if(!checkRange(selectedIndex()))
        return;
    ThumbnailWidget *thumb = thumbnails.at(selectedIndex());
    ensureVisible(thumb, 0, 0);
}

/*void FolderGridView::ensureItemVisible(const QRectF &rect, int xmargin, int ymargin) {
    //Q_D(QGraphicsView);
    qreal width = viewport()->width();
    qreal height = viewport()->height();
    QRectF viewRect = this->matrix().mapRect(rect);
    qreal left = d->horizontalScroll();
    qreal right = left + width;
    qreal top = d->verticalScroll();
    qreal bottom = top + height;
    if (viewRect.left() <= left + xmargin) {
        // need to scroll from the left
        if (!d->leftIndent)
            horizontalScrollBar()->setValue(int(viewRect.left() - xmargin - 0.5));
    }
    if (viewRect.right() >= right - xmargin) {
        // need to scroll from the right
        if (!d->leftIndent)
            horizontalScrollBar()->setValue(int(viewRect.right() - width + xmargin + 0.5));
    }
    if (viewRect.top() <= top + ymargin) {
        // need to scroll from the top
        if (!d->topIndent)
            verticalScrollBar()->setValue(int(viewRect.top() - ymargin - 0.5));
    }
    if (viewRect.bottom() >= bottom - ymargin) {
        // need to scroll from the bottom
        if (!d->topIndent)
            verticalScrollBar()->setValue(int(viewRect.bottom() - height + ymargin + 0.5));
    }
}
*/

void FolderGridView::selectAbove() {
    if(!thumbnails.count())
        return;

    if(checkRange(shiftedIndex)) {
        selectIndex(shiftedIndex);
        focusOn(shiftedIndex);
        shiftedIndex = -1;
        return;
    }

    int index;
    if(!checkRange(selectedIndex())) {
        index = 0;
    } else {
        index = flowLayout->itemAbove(selectedIndex());
    }
    selectIndex(index);
    scrollToCurrent();
}

void FolderGridView::selectBelow() {
    if(!thumbnails.count())
        return;

    int index;
    if(!checkRange(selectedIndex())) {
        index = 0;
    } else {
        index = flowLayout->itemBelow(selectedIndex());
        if(!checkRange(index)) {
            // select last & remember previous
            if(selectedIndex() / flowLayout->columns() < flowLayout->rows() - 1) {
                index = flowLayout->count() - 1;
                shiftedIndex = selectedIndex();
            }
        }
    }

    selectIndex(index);
    scrollToCurrent();
}

void FolderGridView::selectNext() {
    if(!thumbnails.count() || selectedIndex() == thumbnails.count() - 1)
        return;
    shiftedIndex = -1;
    int index = selectedIndex() + 1;
    if(index < 0)
        index = 0;
    else if(index >= thumbnails.count())
        index = thumbnails.count() - 1;

    selectIndex(index);
    scrollToCurrent();
}

void FolderGridView::selectPrev() {
    if(!thumbnails.count() || selectedIndex() == 0)
        return;
    shiftedIndex = -1;
    int index = selectedIndex() - 1;
    if(index < 0)
        index = 0;
    else if(index >= thumbnails.count())
        index = thumbnails.count() - 1;

    selectIndex(index);
    scrollToCurrent();
}

void FolderGridView::pageUp() {
    if(!thumbnails.count())
        return;
    shiftedIndex = -1;
    int index = selectedIndex(), tmp;
    // 4 rows up
    for(int i = 0; i < 4; i++) {
        tmp = flowLayout->itemAbove(index);
        if(checkRange(tmp))
            index = tmp;
    }
    selectIndex(index);
    scrollToCurrent();
}

void FolderGridView::pageDown() {
    if(!thumbnails.count())
        return;
    shiftedIndex = -1;
    int index = selectedIndex(), tmp;
    // 4 rows up
    for(int i = 0; i < 4; i++) {
        tmp = flowLayout->itemBelow(index);
        if(checkRange(tmp)) {
            index = tmp;
        } else {
            index = thumbnails.count() - 1;
        }
    }
    selectIndex(index);    
    scrollToCurrent();
}

void FolderGridView::selectFirst() {
    if(!thumbnails.count())
        return;
    shiftedIndex = -1;
    selectIndex(0);
    scrollToCurrent();
}

void FolderGridView::selectLast() {
    if(!thumbnails.count())
        return;
    shiftedIndex = -1;
    selectIndex(thumbnails.count() - 1);
    scrollToCurrent();
}

void FolderGridView::scrollToCurrent() {
    scrollToItem(selectedIndex());
}

void FolderGridView::scrollToItem(int index) {
    if(!checkRange(index))
        return;

    ThumbnailWidget *item = thumbnails.at(index);

    QRectF sceneRect = mapToScene(viewport()->rect()).boundingRect();
    QRectF itemRect = item->mapRectToScene(item->rect());

    bool visible = sceneRect.contains(itemRect);
    if(!visible) {
        int delta = 0;
        // UP
        if(itemRect.top() >= sceneRect.top())
            delta = sceneRect.bottom() - itemRect.bottom();
        // DOWN
        else
            delta = sceneRect.top() - itemRect.top();
        scrollSmooth(delta);
    }
}

// same as scrollToItem minus the animation
void FolderGridView::focusOn(int index) {
    if(!checkRange(index))
        return;
    ThumbnailWidget *thumb = thumbnails.at(index);
    ensureVisible(thumb, 0, 0);
    loadVisibleThumbnailsDelayed();
}

void FolderGridView::setupLayout() {
    this->setAlignment(Qt::AlignHCenter);

    flowLayout = new FlowLayout();
    flowLayout->setContentsMargins(12,0,12,0);
    setFrameShape(QFrame::NoFrame);
    scene.addItem(&holderWidget);
    holderWidget.setLayout(flowLayout);
}

ThumbnailWidget* FolderGridView::createThumbnailWidget() {
    ThumbnailGridWidget *widget = new ThumbnailGridWidget();
    widget->setDrawLabel(mShowLabels);
    widget->setPadding(8,8);
    widget->setThumbnailSize(this->mThumbnailSize); // TODO: constructor
    return widget;
}

void FolderGridView::addItemToLayout(ThumbnailWidget* widget, int pos) {
    scene.addItem(widget);
    flowLayout->insertItem(pos, widget);
}

void FolderGridView::removeItemFromLayout(int pos) {
    flowLayout->removeAt(pos);
}

void FolderGridView::removeAll() {
    flowLayout->clear();
    qDeleteAll(thumbnails);
    thumbnails.clear();
}

void FolderGridView::updateLayout() {
    shiftedIndex = -1;
    flowLayout->invalidate();
    flowLayout->activate();
}

void FolderGridView::keyPressEvent(QKeyEvent *event) {
    QString shortcut = ShortcutBuilder::fromEvent(event);
    if(reservedShortcuts.contains(shortcut)) {
        if(shortcut == "Right") {
            selectNext();
        }
        if(shortcut == "Left") {
            selectPrev();
        }
        if(shortcut == "Up") {
            selectAbove();
        }
        if(shortcut == "Down") {
            selectBelow();
        }
        if(shortcut == "Return") {
            if(checkRange(selectedIndex()))
                emit thumbnailPressed(selectedIndex());
        }
        if(shortcut == "home") {
            selectFirst();
        }
        if(shortcut == "end") {
            selectLast();
        }
        if(shortcut == "pageUp") {
            pageUp();
        }
        if(shortcut == "pageDown") {
            pageDown();
        }
    } else {
        event->ignore();
    }
}

void FolderGridView::mousePressEvent(QMouseEvent *event) {
    ThumbnailView::mousePressEvent(event);
}

void FolderGridView::mouseReleaseEvent(QMouseEvent *event) {
    event->ignore();
}

void FolderGridView::wheelEvent(QWheelEvent *event) {
    if(event->modifiers().testFlag(Qt::ControlModifier)) {
        if(event->delta() > 0)
            zoomIn();
        else if(event->delta() < 0)
            zoomOut();
    } else {
        ThumbnailView::wheelEvent(event);
    }
}

void FolderGridView::zoomIn() {
    setThumbnailSize(this->mThumbnailSize + ZOOM_STEP);
}

void FolderGridView::zoomOut() {
    setThumbnailSize(this->mThumbnailSize - ZOOM_STEP);
}

void FolderGridView::setThumbnailSize(int newSize) {
    newSize = clamp(newSize, THUMBNAIL_SIZE_MIN, THUMBNAIL_SIZE_MAX);
    mThumbnailSize = newSize;
    for(int i = 0; i < thumbnails.count(); i++) {
        thumbnails.at(i)->setThumbnailSize(newSize);
    }
    updateLayout();
    fitToContents();
    if(checkRange(selectedIndex()))
        ensureVisible(thumbnails.at(selectedIndex()), 0, 40);
    emit thumbnailSizeChanged(mThumbnailSize);
    loadVisibleThumbnails();
}

void FolderGridView::fitToContents() {
    holderWidget.setMinimumSize(size() - QSize(scrollBar->width(), 0));
    holderWidget.setMaximumSize(size() - QSize(scrollBar->width(), 0));
    fitSceneToContents();
}

void FolderGridView::resizeEvent(QResizeEvent *event) {
    if(this->isVisible()) {
        ThumbnailView::resizeEvent(event);
        fitToContents();
        focusOn(selectedIndex());
        loadVisibleThumbnailsDelayed();
    }
}
