#ifndef FOLDERVIEW_H
#define FOLDERVIEW_H

#include <QGraphicsWidget>

#include "gui/customwidgets/thumbnailview.h"
#include "gui/flowlayout.h"

class FolderView : public ThumbnailView
{
    Q_OBJECT
public:
    explicit FolderView(QWidget *parent = nullptr);

public slots:

private:
    FlowLayout *flowLayout;
    QGraphicsWidget holderWidget;

private slots:

protected:
    void resizeEvent(QResizeEvent *event);
    void addItemToLayout(ThumbnailLabel *widget, int pos);
    void removeItemFromLayout(int pos);
    void setupLayout();
    ThumbnailLabel *createThumbnailWidget();
    void onPopulate();

signals:

};

#endif // FOLDERVIEW_H
