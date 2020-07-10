#include "treeviewcustom.h"

TreeViewCustom::TreeViewCustom(QWidget *parent) : QTreeView(parent) {
    setAcceptDrops(true);
    setDropIndicatorShown(true);
    setDragEnabled(true);

    // proxy scrollbar
    this->verticalScrollBar()->setStyleSheet("max-width: 0px;");
    overlayScrollbar.setParent(this);
    overlayScrollbar.setStyleSheet("background-color: transparent;");
    connect(verticalScrollBar(), &QScrollBar::rangeChanged, &overlayScrollbar, &QScrollBar::setRange);
    connect(verticalScrollBar(), &QScrollBar::valueChanged, &overlayScrollbar, &QScrollBar::setValue);
    connect(&overlayScrollbar, &QScrollBar::valueChanged, [this]() {
        this->verticalScrollBar()->setValue(overlayScrollbar.value());
    });
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

void TreeViewCustom::showEvent(QShowEvent *event) {
    QTreeView::showEvent(event);
}

void TreeViewCustom::enterEvent(QEvent *event) {
    QTreeView::enterEvent(event);
    updateScrollbarStyle();
}
void TreeViewCustom::leaveEvent(QEvent *event) {
    QTreeView::leaveEvent(event);
    updateScrollbarStyle();
}

QSize TreeViewCustom::minimumSizeHint() const {
    QSize sz(QTreeView::minimumSizeHint().width(), 0);
    return sz;
}

void TreeViewCustom::resizeEvent(QResizeEvent *event) {
    QTreeView::resizeEvent(event);
    updateScrollbarStyle();
}

void TreeViewCustom::updateScrollbarStyle() {
    QString handle, hover = settings->colorScheme().scrollbar_hover.name();
    if(rect().contains(mapFromGlobal(cursor().pos())))
        handle = settings->colorScheme().scrollbar.name();
    else
        handle = settings->colorScheme().folderview_separator.name();
    overlayScrollbar.setGeometry(width() - SCROLLBAR_WIDTH, 0, SCROLLBAR_WIDTH, height());
    overlayScrollbar.setStyleSheet( "QScrollBar { background-color: transparent; } QScrollBar::handle:vertical { background-color: "+ handle +" } QScrollBar::handle:vertical:hover { background-color: " + hover + " }" );

    overlayScrollbar.setVisible( (this->verticalScrollBar()->maximum()) );
}
