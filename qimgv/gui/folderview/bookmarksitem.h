#ifndef BOOKMARKSITEM_H
#define BOOKMARKSITEM_H

#include <QObject>
#include <QWidget>
#include <QStyleOption>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QLabel>
#include "gui/customwidgets/iconbutton.h"
#include "gui/customwidgets/clickablelabel.h"
#include <QMouseEvent>

class BookmarksItem : public QWidget {
    Q_OBJECT
public:
    explicit BookmarksItem(QString _dirName, QString _dirPath, QWidget *parent = nullptr);
    QString path();

signals:
    void clicked(QString dirPath);
    void removeClicked(QString dirPath);

protected:
    void mouseReleaseEvent(QMouseEvent *event);
    bool eventFilter(QObject *o, QEvent *ev) Q_DECL_OVERRIDE;

    void paintEvent(QPaintEvent *event) override;
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;

private slots:
    void onRemoveClicked();
private:
    QString dirName, dirPath;

    QLabel folderIconLabel;
    IconButton removeIconLabel;
    QLabel dirNameLabel;
    QSpacerItem *spacer;
    QHBoxLayout layout;
};

#endif // BOOKMARKSITEM_H
