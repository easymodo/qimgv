#pragma once

#include <QGraphicsWidget>

#include "gui/customwidgets/thumbnailview.h"
#include "gui/folderview/thumbnailgridwidget.h"
#include "gui/flowlayout.h"
#include "utils/stuff.h"
#include "components/actionmanager/actionmanager.h"

class FolderGridView : public ThumbnailView {
    Q_OBJECT
public:
    explicit FolderGridView(QWidget *parent = nullptr);

    const int THUMBNAIL_SIZE_MIN = 80;  // px
    const int THUMBNAIL_SIZE_MAX = 400;  // these should be divisible by ZOOM_STEP
    const int ZOOM_STEP = 20;
    void selectAll();

public slots:
    void show();
    void hide();

    void selectFirst();
    void selectLast();
    void pageUp();
    void pageDown();
    void selectAbove();
    void selectBelow();
    void selectNext();
    void selectPrev();

    void zoomIn();
    void zoomOut();
    void setThumbnailSize(int newSize);
    void setShowLabels(bool mode);
    virtual void focusOn(int index);
    virtual void setDragHover(int index);

private:
    FlowLayout *flowLayout;
    QGraphicsWidget holderWidget;
    int shiftedCol;
    bool mShowLabels;
    void scrollToCurrent();
    void scrollToItem(int index);
    int lastDragTarget = -1;

private slots:
    void onitemSelected();

protected:
    void resizeEvent(QResizeEvent *event);
    virtual void updateScrollbarIndicator();
    void addItemToLayout(ThumbnailWidget *widget, int pos);
    void removeItemFromLayout(int pos);
    void removeAll();
    void setupLayout();
    ThumbnailWidget *createThumbnailWidget();
    void updateLayout();
    void ensureSelectedItemVisible();
    virtual void fitSceneToContents() override;

    void keyPressEvent(QKeyEvent *event);
    void wheelEvent(QWheelEvent *event);    
    void dropEvent(QDropEvent *event);

    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
signals:
    void thumbnailSizeChanged(int);
    void showLabelsChanged(bool);
};
