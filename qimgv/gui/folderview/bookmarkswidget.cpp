#include "bookmarkswidget.h"

BookmarksWidget::BookmarksWidget(QWidget *parent) : QWidget(parent) {
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
    addBookmark(QDir::homePath());
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
}

void BookmarksWidget::removeBookmark(QString dirPath) {
    for(int i = 0; i < layout.count(); i++) {
        auto w = dynamic_cast<BookmarksItem*>(layout.itemAt(i)->widget());
        if(w && w->path() == dirPath) {
            layout.removeWidget(w);
            disconnect(w, &BookmarksItem::clicked, this, &BookmarksWidget::bookmarkClicked);
            disconnect(w, &BookmarksItem::removeClicked, this, &BookmarksWidget::removeBookmark);
            w->deleteLater();
            paths.removeAll(dirPath);
            break;
        }
    }
}
