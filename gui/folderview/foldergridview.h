#ifndef FOLDERGRIDVIEW_H
#define FOLDERGRIDVIEW_H

#include <QGraphicsWidget>

#include "gui/customwidgets/thumbnailview.h"
#include "gui/folderview/thumbnailgridwidget.h"
#include "gui/flowlayout.h"
#include "components/actionmanager/actionmanager.h"

class FolderGridView : public ThumbnailView
{
    Q_OBJECT
public:
    explicit FolderGridView(QWidget *parent = nullptr);

public slots:
    void show();
    void hide();

    void selectFirst();
    void selectLast();
    void selectIndex(int index);
private:
    FlowLayout *flowLayout;
    QGraphicsWidget holderWidget;
    QStringList allowedKeys;
    int selectedIndex;

private slots:
    void selectAbove();
    void selectBelow();
    void selectNext();
    void selectPrev();

protected:
    void resizeEvent(QResizeEvent *event);
    void addItemToLayout(ThumbnailWidget *widget, int pos);
    void removeItemFromLayout(int pos);
    void setupLayout();
    ThumbnailWidget *createThumbnailWidget();
    void onPopulate();

    void keyPressEvent(QKeyEvent *event);

signals:

};

#endif // FOLDERGRIDVIEW_H
