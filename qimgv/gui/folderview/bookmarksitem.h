#ifndef BOOKMARKSITEM_H
#define BOOKMARKSITEM_H

#include <QObject>
#include <QWidget>
#include <QStyleOption>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QMimeData>
#include <QLabel>
#include "gui/customwidgets/iconbutton.h"
#include "gui/customwidgets/clickablelabel.h"
#include "settings.h"
#include <QMouseEvent>

class BookmarksItem : public QWidget {
    Q_OBJECT
public:
    explicit BookmarksItem(QString _dirName, QString _dirPath, QWidget *parent = nullptr);
    QString path();

public slots:
    void setHighlighted(bool mode);

signals:
    void clicked(QString dirPath);
    void removeClicked(QString dirPath);
    void droppedIn(QList<QString> paths, QString dirPath);

protected:
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

    void paintEvent(QPaintEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragLeaveEvent(QDragLeaveEvent *event) override;

private slots:
    void onRemoveClicked();

private:
    QString dirName, dirPath;

    IconWidget folderIconWidget;
    IconButton removeItemButton;
    QLabel dirNameLabel;
    QSpacerItem *spacer;
    QHBoxLayout layout;
    bool mHighlighted;
};

#endif // BOOKMARKSITEM_H
