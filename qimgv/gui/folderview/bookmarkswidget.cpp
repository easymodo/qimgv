#include "bookmarkswidget.h"

BookmarksWidget::BookmarksWidget(QWidget *parent) : QWidget(parent), highlightedPath("") {
    setAcceptDrops(true);
    setContentsMargins(0,0,0,0);
    layout.setContentsMargins(0,0,0,0);
    layout.setSpacing(0);
    setLayout(&layout);
    connect(settings, &Settings::settingsChanged, this, &BookmarksWidget::readSettings);
    readSettings();
}

BookmarksWidget::~BookmarksWidget() {
}

void BookmarksWidget::readSettings() {
    QStringList _paths = settings->bookmarks();
    for(auto path : _paths)
        addBookmark(path);
    if(_paths.empty())
        addBookmark(QDir::homePath());
}

void BookmarksWidget::saveBookmarks() {
    settings->setBookmarks(paths);
}

void BookmarksWidget::addBookmark(QString dirPath) {
    if(paths.contains(dirPath))
        return;
    paths.push_back(dirPath);
    QUrl url(dirPath);
    BookmarksItem *item = new BookmarksItem(url.fileName(), dirPath);
    layout.addWidget(item);
    connect(item, &BookmarksItem::clicked, this, &BookmarksWidget::bookmarkClicked);
    connect(item, &BookmarksItem::removeClicked, this, &BookmarksWidget::removeBookmark);
    connect(item, &BookmarksItem::droppedIn, this, &BookmarksWidget::droppedIn);
    saveBookmarks();
}

void BookmarksWidget::removeBookmark(QString dirPath) {
    for(int i = 0; i < layout.count(); i++) {
        auto w = dynamic_cast<BookmarksItem*>(layout.itemAt(i)->widget());
        if(w && w->path() == dirPath) {
            if(highlightedPath == dirPath)
                highlightedPath = "";
            layout.removeWidget(w);
            disconnect(w, &BookmarksItem::clicked, this, &BookmarksWidget::bookmarkClicked);
            disconnect(w, &BookmarksItem::removeClicked, this, &BookmarksWidget::removeBookmark);
            disconnect(w, &BookmarksItem::droppedIn, this, &BookmarksWidget::droppedIn);
            w->deleteLater();
            paths.removeAll(dirPath);
            saveBookmarks();
            break;
        }
    }
}

void BookmarksWidget::onPathChanged(QString path) {
    if(highlightedPath == path)
        return;
    if(paths.contains(highlightedPath)) {
        int currentIndex = paths.indexOf(highlightedPath);
        auto w = dynamic_cast<BookmarksItem*>(layout.itemAt(currentIndex)->widget());
        w->setHighlighted(false);
        highlightedPath = "";
    }
    if(paths.contains(path)) {
        int newIndex = paths.indexOf(path);
        auto w = dynamic_cast<BookmarksItem*>(layout.itemAt(newIndex)->widget());
        w->setHighlighted(true);
        highlightedPath = path;
    }
}

void BookmarksWidget::dropEvent(QDropEvent *event) {
//    QModelIndex dropIndex = indexAt(event->pos());
//    if(dropIndex.isValid())
//        emit droppedIn(event->mimeData()->urls(), dropIndex);
}

void BookmarksWidget::dragEnterEvent(QDragEnterEvent *event) {
    if(event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}
