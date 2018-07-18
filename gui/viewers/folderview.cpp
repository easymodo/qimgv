#include "folderview.h"

// TODO: create a base class for this and the one on panel

FolderView::FolderView(QWidget *parent)
    : ThumbnailView(THUMBNAILVIEW_VERTICAL, parent)
{
    setupLayout();

    // tmp
    populate(40);
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
    widget->setMargins(2,2);
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
}

void FolderView::resizeEvent(QResizeEvent *event) {
    QGraphicsView::resizeEvent(event);

    holderWidget.setMinimumSize(size());
    holderWidget.setMaximumSize(size());

    fitSceneToContents();

    loadVisibleThumbnails();
    //qDebug() << this->rect() << holderWidget.size() << scene.sceneRect();
}
