#ifndef BOOKMARKSWIDGET_H
#define BOOKMARKSWIDGET_H

#include <QWidget>
#include "gui/folderview/bookmarksitem.h"
#include <QLabel>
#include <QVBoxLayout>
#include "settings.h"

class BookmarksWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BookmarksWidget(QWidget *parent = nullptr);
    ~BookmarksWidget();

public slots:
    void addBookmark(QString directoryPath);

    void removeBookmark(QString dirPath);
private slots:
    void readSettings();

signals:
    void bookmarkClicked(QString dirPath);

private:
    QVBoxLayout layout;
    QStringList paths;
};

#endif // BOOKMARKSWIDGET_H
