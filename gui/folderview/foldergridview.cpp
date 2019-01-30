#include "foldergridview.h"

// TODO: create a base class for this and the one on panel

FolderGridView::FolderGridView(QWidget *parent)
    : ThumbnailView(THUMBNAILVIEW_VERTICAL, parent),
      selectedIndex(-1),
      shiftedIndex(-1),
      mShowLabels(false)
{
    this->viewport()->setAttribute(Qt::WA_OpaquePaintEvent, true);
    this->scene.setBackgroundBrush(QColor(50,50,51));
    setupLayout();
    allowedKeys << "Up"
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
    if(!checkRange(selectedIndex))
        return;
    ThumbnailWidget *thumb = thumbnails.at(selectedIndex);
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
        shiftedIndex = -1;
        return;
    }

    int index;
    if(!checkRange(selectedIndex)) {
        index = 0;
    } else {
        index = flowLayout->itemAbove(selectedIndex);
    }
    selectIndex(index);
}

void FolderGridView::selectBelow() {
    if(!thumbnails.count())
        return;

    int index;
    if(!checkRange(selectedIndex)) {
        index = 0;
    } else {
        index = flowLayout->itemBelow(selectedIndex);
        if(!checkRange(index)) {
            // select last & remember previous
            if(selectedIndex / flowLayout->columns() < flowLayout->rows() - 1) {
                index = flowLayout->count() - 1;
                shiftedIndex = selectedIndex;
            }
        }
    }
    selectIndex(index);
}

void FolderGridView::selectNext() {
    if(!thumbnails.count() || selectedIndex == thumbnails.count() - 1)
        return;
    shiftedIndex = -1;
    int index = selectedIndex + 1;
    if(index < 0)
        index = 0;
    else if(index >= thumbnails.count())
        index = thumbnails.count() - 1;

    selectIndex(index);
}

void FolderGridView::selectPrev() {
    if(!thumbnails.count() || selectedIndex == 0)
        return;
    shiftedIndex = -1;
    int index = selectedIndex - 1;
    if(index < 0)
        index = 0;
    else if(index >= thumbnails.count())
        index = thumbnails.count() - 1;

    selectIndex(index);
}

void FolderGridView::pageUp() {
    if(!thumbnails.count())
        return;
    shiftedIndex = -1;
    int index = selectedIndex, tmp;
    // 4 rows up
    for(int i = 0; i < 4; i++) {
        tmp = flowLayout->itemAbove(index);
        if(checkRange(tmp))
            index = tmp;
    }
    selectIndex(index);
}

void FolderGridView::pageDown() {
    if(!thumbnails.count())
        return;
    shiftedIndex = -1;
    int index = selectedIndex, tmp;
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
}

void FolderGridView::selectFirst() {
    if(!thumbnails.count())
        return;
    shiftedIndex = -1;
    selectIndex(0);
}

void FolderGridView::selectLast() {
    if(!thumbnails.count())
        return;
    shiftedIndex = -1;
    selectIndex(thumbnails.count() - 1);
}

void FolderGridView::selectIndex(int index) {
    if(!checkRange(index))
        return;

    if(checkRange(selectedIndex))
        thumbnails.at(selectedIndex)->setHighlighted(false, false);
    selectedIndex = index;

    ThumbnailWidget *thumb = thumbnails.at(selectedIndex);
    thumb->setHighlighted(true, false);
    ensureVisible(thumb, 0, 0);
    loadVisibleThumbnails();
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
    widget->setPadding(7,7);
    return widget;
}

// TODO: insert
void FolderGridView::addItemToLayout(ThumbnailWidget* widget, int pos) {
    scene.addItem(widget);
    flowLayout->insertItem(pos, widget);
}

void FolderGridView::removeItemFromLayout(int pos) {
    flowLayout->removeAt(pos);
}

void FolderGridView::updateLayout() {
    shiftedIndex = -1;
    flowLayout->invalidate();
    flowLayout->activate();
    if(!thumbnails.count())
        selectedIndex = -1;
}

void FolderGridView::keyPressEvent(QKeyEvent *event) {
    quint32 nativeScanCode = event->nativeScanCode();
    QString key = actionManager->keyForNativeScancode(nativeScanCode);
    if(allowedKeys.contains(key)) {
        if(key == "Right") {
            selectNext();
        }
        if(key == "Left") {
            selectPrev();
        }
        if(key == "Up") {
            selectAbove();
        }
        if(key == "Down") {
            selectBelow();
        }
        if(key == "Return") {
            if(checkRange(selectedIndex))
                emit thumbnailPressed(selectedIndex);
        }
        if(key == "home") {
            selectFirst();
        }
        if(key == "end") {
            selectLast();
        }
        if(key == "pageUp") {
            pageUp();
        }
        if(key == "pageDown") {
            pageDown();
        }
    } else {
        event->ignore();
    }
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
    if(checkRange(selectedIndex))
        ensureVisible(thumbnails.at(selectedIndex), 0, 40);
    emit thumbnailSizeChanged(mThumbnailSize);
    loadVisibleThumbnails();
}

void FolderGridView::fitToContents() {
    holderWidget.setMinimumSize(size() - QSize(scrollBar->width(), 0));
    holderWidget.setMaximumSize(size() - QSize(scrollBar->width(), 0));
    fitSceneToContents();
}

void FolderGridView::resizeEvent(QResizeEvent *event) {
    QGraphicsView::resizeEvent(event);
    fitToContents();
    loadVisibleThumbnailsDelayed();
}
