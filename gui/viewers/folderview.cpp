#include "folderview.h"

// TODO: create a base class for this and the one on panel

FolderView::FolderView(QWidget *parent)
    : ThumbnailView(THUMBNAILVIEW_VERTICAL, parent)
{
    setupLayout();
}

void FolderView::setupLayout() {
    flowLayout = new FlowLayout();
    flowLayout->setContentsMargins(20,20,20,20);
    setFrameShape(QFrame::NoFrame);
    scene.addItem(&holderWidget);

    // tmp
    populate(100);
    holderWidget.setLayout(flowLayout);
}

// set up widget's options here before adding to layout
ThumbnailLabel* FolderView::createThumbnailWidget() {
    ThumbnailLabel *widget = new ThumbnailLabel();
    widget->setDrawLabel(false);
    widget->setHightlightStyle(HIGHLIGHT_BACKGROUND);
    widget->setMargins(4,8);
    return widget;
}

// TODO: insert
void FolderView::addItemToLayout(ThumbnailLabel* widget, int pos) {
    flowLayout->addItem(widget);
}

void FolderView::removeItemFromLayout(int pos) {
    flowLayout->removeAt(pos);
}

void FolderView::resizeEvent(QResizeEvent *event) {
    QGraphicsView::resizeEvent(event);
    holderWidget.setMinimumSize(size());
    holderWidget.setMaximumSize(size());
    QRectF rect = scene.itemsBoundingRect();
    scene.setSceneRect(rect);
}
