#ifndef FOLDERVIEW_H
#define FOLDERVIEW_H

#include <QGraphicsWidget>

#include "gui/customwidgets/thumbnailview.h"
#include "gui/flowlayout.h"
#include "components/actionmanager/actionmanager.h"

class FolderView : public ThumbnailView
{
    Q_OBJECT
public:
    explicit FolderView(QWidget *parent = nullptr);

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
    void addItemToLayout(ThumbnailLabel *widget, int pos);
    void removeItemFromLayout(int pos);
    void setupLayout();
    ThumbnailLabel *createThumbnailWidget();
    void onPopulate();

    void keyPressEvent(QKeyEvent *event);

signals:

};

#endif // FOLDERVIEW_H
