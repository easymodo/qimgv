#include "treeviewcustom.h"

TreeViewCustom::TreeViewCustom(QWidget *parent) : QTreeView(parent) {
    setAcceptDrops(true);
    setDropIndicatorShown(true);
    setDragEnabled(true);
}

void TreeViewCustom::dropEvent(QDropEvent *event) {
    QModelIndex dropIndex = indexAt(event->pos());
    if(dropIndex.isValid())
        emit droppedIn(event->mimeData()->urls(), dropIndex);
}

void TreeViewCustom::dragEnterEvent(QDragEnterEvent *event) {
    if(event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}
