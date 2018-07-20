#include "foldergridview.h"

// TODO: create a base class for this and the one on panel

FolderGridView::FolderGridView(QWidget *parent)
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
}

void FolderGridView::show() {
    ThumbnailView::show();
    setFocus();
}

void FolderGridView::hide() {
    ThumbnailView::hide();
    clearFocus();
}

void FolderGridView::selectAbove() {
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

void FolderGridView::selectBelow() {
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

void FolderGridView::selectNext() {
    if(!thumbnails.count())
        return;

    int index = selectedIndex + 1;
    if(index < 0)
        index = 0;
    else if(index >= thumbnails.count())
        index = thumbnails.count() - 1;

    selectIndex(index);
}

void FolderGridView::selectPrev() {
    if(!thumbnails.count())
        return;

    int index = selectedIndex - 1;
    if(index < 0)
        index = 0;
    else if(index >= thumbnails.count())
        index = thumbnails.count() - 1;

    selectIndex(index);
}

void FolderGridView::selectFirst() {
    if(!thumbnails.count())
        return;

    selectIndex(0);
}

void FolderGridView::selectLast() {
    if(!thumbnails.count())
        return;

    selectIndex(thumbnails.count() - 1);
}

void FolderGridView::selectIndex(int index) {
    if(!checkRange(index))
        return;

    if(checkRange(selectedIndex))
        thumbnails.at(selectedIndex)->setHighlighted(false, true);
    selectedIndex = index;

    ThumbnailWidget *thumb = thumbnails.at(selectedIndex);
    thumb->setHighlighted(true, false);
    ensureVisible(thumb, 0, 0);
    loadVisibleThumbnails();
}

void FolderGridView::setupLayout() {
    this->setAlignment(Qt::AlignHCenter);

    flowLayout = new FlowLayout();
    flowLayout->setContentsMargins(0,0,0,0);
    setFrameShape(QFrame::NoFrame);
    scene.addItem(&holderWidget);
    holderWidget.setLayout(flowLayout);
}

ThumbnailWidget* FolderGridView::createThumbnailWidget() {
    // important: parent must be set, otherwise widget won't be drawn
    ThumbnailGridWidget *widget = new ThumbnailGridWidget(&holderWidget);
    widget->setDrawLabel(true);
    widget->setMargins(3,3);
    return widget;
}

// TODO: insert
void FolderGridView::addItemToLayout(ThumbnailWidget* widget, int pos) {
    flowLayout->addItem(widget);
}

void FolderGridView::removeItemFromLayout(int pos) {
    flowLayout->removeAt(pos);
}

void FolderGridView::onPopulate() {
    flowLayout->activate();
    if(thumbnails.count())
        selectedIndex = -1;
}

void FolderGridView::keyPressEvent(QKeyEvent *event) {
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

void FolderGridView::resizeEvent(QResizeEvent *event) {
    QGraphicsView::resizeEvent(event);

    holderWidget.setMinimumSize(size() - QSize(scrollBar->width(), 0));
    holderWidget.setMaximumSize(size() - QSize(scrollBar->width(), 0));
    fitSceneToContents();

    loadVisibleThumbnails();

    //qDebug() << this->rect() << holderWidget.size() << scene.sceneRect();
}
