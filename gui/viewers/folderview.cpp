#include "folderview.h"

// TODO: create a base class for this and the one on panel

FolderView::FolderView(QWidget *parent)
    : ThumbnailView(THUMBNAILVIEW_VERTICAL, parent),
      selectedIndex(-1)
{
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

    // tmp
    populate(40);
}

void FolderView::show() {
    ThumbnailView::show();
    setFocus();
}

void FolderView::hide() {
    ThumbnailView::hide();
    clearFocus();
}

void FolderView::selectAbove() {
    if(!thumbnails.count())
        return;

    int index;
    if(!checkRange(selectedIndex)) {
        index = 0;
    } else {
        index = flowLayout->itemAbove(selectedIndex);
    }
    selectIndex(index);
}

void FolderView::selectBelow() {
    if(!thumbnails.count())
        return;

    int index;
    if(!checkRange(selectedIndex)) {
        index = 0;
    } else {
        index = flowLayout->itemBelow(selectedIndex);
    }
    selectIndex(index);
}

void FolderView::selectNext() {
    if(!thumbnails.count())
        return;

    int index = selectedIndex + 1;
    if(index < 0)
        index = 0;
    else if(index >= thumbnails.count())
        index = thumbnails.count() - 1;

    selectIndex(index);
}

void FolderView::selectPrev() {
    if(!thumbnails.count())
        return;

    int index = selectedIndex - 1;
    if(index < 0)
        index = 0;
    else if(index >= thumbnails.count())
        index = thumbnails.count() - 1;

    selectIndex(index);
}

void FolderView::selectFirst() {
    if(!thumbnails.count())
        return;

    selectIndex(0);
}

void FolderView::selectLast() {
    if(!thumbnails.count())
        return;

    selectIndex(thumbnails.count() - 1);
}

void FolderView::selectIndex(int index) {
    if(!checkRange(index))
        return;

    if(checkRange(selectedIndex))
        thumbnails.at(selectedIndex)->setHighlighted(false, false);
    selectedIndex = index;

    ThumbnailLabel *thumb = thumbnails.at(selectedIndex);
    thumb->setHighlighted(true, false);
    ensureVisible(thumb, 0, 0);
    loadVisibleThumbnails();
}

void FolderView::setupLayout() {
    this->setAlignment(Qt::AlignHCenter);

    flowLayout = new FlowLayout();
    flowLayout->setContentsMargins(20,20,20,20);
    setFrameShape(QFrame::NoFrame);
    scene.addItem(&holderWidget);
    holderWidget.setLayout(flowLayout);
}

ThumbnailLabel* FolderView::createThumbnailWidget() {
    // important: parent must be set, otherwise widget won't be drawn
    ThumbnailLabel *widget = new ThumbnailLabel(&holderWidget);
    widget->setDrawLabel(false);
    widget->setHightlightStyle(HIGHLIGHT_BACKGROUND);
    widget->setMargins(3,3);
    return widget;
}

// TODO: insert
void FolderView::addItemToLayout(ThumbnailLabel* widget, int pos) {
    flowLayout->addItem(widget);
}

void FolderView::removeItemFromLayout(int pos) {
    flowLayout->removeAt(pos);
}

void FolderView::onPopulate() {
    flowLayout->activate();
    if(thumbnails.count())
        selectedIndex = -1;
}

void FolderView::keyPressEvent(QKeyEvent *event) {
    int nativeScanCode = event->nativeScanCode();
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

        }
        if(key == "pageDown") {

        }
    } else {
        event->ignore();
    }
}

void FolderView::resizeEvent(QResizeEvent *event) {
    QGraphicsView::resizeEvent(event);

    holderWidget.setMinimumSize(size());
    holderWidget.setMaximumSize(size());

    fitSceneToContents();

    loadVisibleThumbnails();

    //qDebug() << this->verticalScrollBar()->width();
    qDebug() << this->rect() << holderWidget.size() << scene.sceneRect();
}
