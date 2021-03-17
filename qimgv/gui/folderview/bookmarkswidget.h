#ifndef BOOKMARKSWIDGET_H
#define BOOKMARKSWIDGET_H

#include <QWidget>
#include "gui/folderview/bookmarksitem.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QMimeData>
#include "settings.h"

class BookmarksWidget : public QWidget {
    Q_OBJECT

public:
    explicit BookmarksWidget(QWidget *parent = nullptr);
    ~BookmarksWidget();

public slots:
    void addBookmark(QString directoryPath);

    void removeBookmark(QString dirPath);
    void onPathChanged(QString path);
private slots:
    void readSettings();

    void saveBookmarks();
signals:
    void bookmarkClicked(QString dirPath);
    void droppedIn(QList<QString> paths, QString dirPath);

protected:
    void dropEvent(QDropEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
private:
    QVBoxLayout layout;
    QStringList paths;
    QString highlightedPath;
};

#endif // BOOKMARKSWIDGET_H
