#include "foldergridview.h"

// TODO: create a base class for this and the one on panel

FolderGridView::FolderGridView(QWidget *parent)
    : ThumbnailView(THUMBNAILVIEW_VERTICAL, parent),
      shiftedCol(-1),
      mShowLabels(false)
{
    offscreenPreloadArea = 2300;

    this->viewport()->setAttribute(Qt::WA_OpaquePaintEvent, true);
    this->scene.setBackgroundBrush(settings->colorScheme().folderview);
    this->setCacheMode(QGraphicsView::CacheBackground);

    // turn this off until [multi]selection is implemented
    setDrawScrollbarIndicator(false);
    setSelectMode(SELECT_BY_DOUBLECLICK);

    connect(settings, &Settings::settingsChanged, [this]() {
        this->scene.setBackgroundBrush(settings->colorScheme().folderview);
    });

    setupLayout();
    connect(this, &ThumbnailView::itemActivated,
            this, &FolderGridView::onitemSelected);
}

void FolderGridView::onitemSelected() {
    shiftedCol = -1;
}

void FolderGridView::updateScrollbarIndicator() {
    if(!thumbnails.count() || selectedIndex() == -1)
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
    fitSceneToContents();
    ensureSelectedItemVisible();
    emit showLabelsChanged(mShowLabels);
}

void FolderGridView::ensureSelectedItemVisible() {
    if(!checkRange(selectedIndex()))
        return;
    ThumbnailWidget *thumb = thumbnails.at(selectedIndex());
    ensureVisible(thumb, 0, 0);
}

void FolderGridView::selectAbove() {
    if(!thumbnails.count() || flowLayout->sameRow(0, selectedIndex()))
        return;
    int newIndex;
    newIndex = flowLayout->itemAbove(selectedIndex());
    if(shiftedCol >= 0) {
        int diff = shiftedCol - flowLayout->columnOf(selectedIndex());
        newIndex += diff;
        shiftedCol = -1;
    }
    if(!checkRange(newIndex))
        newIndex = 0;
    selectIndex(newIndex);
    scrollToCurrent();
}

void FolderGridView::selectBelow() {
    if(!thumbnails.count() || flowLayout->sameRow(selectedIndex(), thumbnails.count() - 1))
        return;
    shiftedCol = -1;
    int newIndex = flowLayout->itemBelow(selectedIndex());
    if(!checkRange(newIndex))
        newIndex = thumbnails.count() - 1;
    if(flowLayout->columnOf(newIndex) != flowLayout->columnOf(selectedIndex()))
        shiftedCol = flowLayout->columnOf(selectedIndex());
    selectIndex(newIndex);
    scrollToCurrent();
}

void FolderGridView::selectNext() {
    if(!thumbnails.count() || selectedIndex() == thumbnails.count() - 1)
        return;
    shiftedCol = -1;
    int newIndex = selectedIndex() + 1;
    if(!checkRange(newIndex))
        newIndex = thumbnails.count() - 1;
    selectIndex(newIndex);
    scrollToCurrent();
}

void FolderGridView::selectPrev() {
    if(!thumbnails.count() || selectedIndex() == 0)
        return;
    shiftedCol = -1;
    int newIndex = selectedIndex() - 1;
    if(!checkRange(newIndex))
        newIndex = 0;
    selectIndex(newIndex);
    scrollToCurrent();
}

void FolderGridView::pageUp() {
    if(!thumbnails.count() || flowLayout->sameRow(0, selectedIndex()))
        return;
    int newIndex = selectedIndex();
    int tmp;
    // 4 rows up
    for(int i = 0; i < 4; i++) {
        tmp = flowLayout->itemAbove(newIndex);
        if(checkRange(tmp))
            newIndex = tmp;
    }
    if(shiftedCol >= 0) {
        int diff = shiftedCol - flowLayout->columnOf(newIndex);
        newIndex += diff;
        shiftedCol = -1;
    }
    selectIndex(newIndex);
    scrollToCurrent();
}

void FolderGridView::pageDown() {
    if(!thumbnails.count() || flowLayout->sameRow(selectedIndex(), thumbnails.count() - 1))
        return;
    shiftedCol = -1;
    int newIndex = selectedIndex();
    int tmp;
    // 4 rows down
    for(int i = 0; i < 4; i++) {
        tmp = flowLayout->itemBelow(newIndex);
        if(checkRange(tmp))
            newIndex = tmp;
    }
    if(flowLayout->columnOf(newIndex) != flowLayout->columnOf(selectedIndex()))
        shiftedCol = flowLayout->columnOf(selectedIndex());
    selectIndex(newIndex);
    scrollToCurrent();
}

void FolderGridView::selectFirst() {
    if(!thumbnails.count())
        return;
    shiftedCol = -1;
    selectIndex(0);
    scrollToCurrent();
}

void FolderGridView::selectLast() {
    if(!thumbnails.count())
        return;
    shiftedCol = -1;
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
    flowLayout->setContentsMargins(9,0,9,0);
    setFrameShape(QFrame::NoFrame);
    scene.addItem(&holderWidget);
    holderWidget.setLayout(flowLayout);
    holderWidget.setContentsMargins(0,0,0,0);
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
    shiftedCol = -1;
    flowLayout->invalidate();
    flowLayout->activate();
}

void FolderGridView::keyPressEvent(QKeyEvent *event) {
    QString shortcut = ShortcutBuilder::fromEvent(event);
    if(shortcut == "Right")
        selectNext();
    else if(shortcut == "Left")
        selectPrev();
    else if(shortcut == "Up")
        selectAbove();
    else if(shortcut == "Down")
        selectBelow();
    else if(shortcut == "Enter")
        emit itemActivated(selectedIndex());
    else if(shortcut == "Home")
        selectFirst();
    else if(shortcut == "End")
        selectLast();
    else if(shortcut == "PgUp")
        pageUp();
    else if(shortcut == "PgDown")
        pageDown();
    else
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
    fitSceneToContents();
    if(checkRange(selectedIndex()))
        ensureVisible(thumbnails.at(selectedIndex()), 0, 40);
    emit thumbnailSizeChanged(mThumbnailSize);
    loadVisibleThumbnails();
}

void FolderGridView::fitSceneToContents() {
    if(scrollBar->isVisible())
        holderWidget.setGeometry(0,0, width() - scrollBar->width(), height());
    else
        holderWidget.setGeometry(0,0, width(), height());
    ThumbnailView::fitSceneToContents();
}

void FolderGridView::resizeEvent(QResizeEvent *event) {
    if(this->isVisible()) {
        ThumbnailView::resizeEvent(event);
        fitSceneToContents();
        //focusOn(selectedIndex());
        loadVisibleThumbnailsDelayed();
    }
}
