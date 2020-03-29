#ifndef TREEVIEWCUSTOM_H
#define TREEVIEWCUSTOM_H

#include <QTreeView>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QDebug>

class TreeViewCustom : public QTreeView
{
    Q_OBJECT
public:
    TreeViewCustom(QWidget *parent = nullptr);
    virtual QSize minimumSizeHint() const override;

signals:
    void droppedIn(QList<QUrl>, QModelIndex);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
};

#endif // TREEVIEWCUSTOM_H
