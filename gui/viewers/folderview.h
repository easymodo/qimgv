#ifndef FOLDERVIEW_H
#define FOLDERVIEW_H

#include <QWidget>
#include <QGraphicsView>
#include <QVBoxLayout>
#include <QGraphicsWidget>
#include <QGraphicsLinearLayout>
#include <QLabel>
#include <QGraphicsProxyWidget>
#include "gui/panels/mainpanel/thumbnaillabel.h"
#include "gui/flowlayout.h"

class FolderView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit FolderView(QWidget *parent = nullptr);
    ~FolderView();
signals:

public slots:
    void populate(int count);
    void setThumbnail(int pos, Thumbnail *thumb);
    void show();

private:
    QVBoxLayout *layout;
    QGraphicsScene scene;
    QGraphicsView *view;
    FlowLayout *flowLayout;
    //QGraphicsLinearLayout *flowLayout;
    QGraphicsWidget holderWidget;

    QList<ThumbnailLabel*> thumbnails;


};

#endif // FOLDERVIEW_H
