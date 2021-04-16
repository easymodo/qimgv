#pragma once

/* This class manages QGraphicsScene, ThumbnailWidget list,
 * scrolling, requesting and setting thumbnails.
 * It doesn't do actual positioning of thumbnails within the scene.
 * (But maybe it should?)
 *
 * Usage: subclass, implement layout-related stuff
 */

#include <QGraphicsView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollBar>
#include <QWheelEvent>
#include <QTimeLine>
#include <QTimer>
#include <QElapsedTimer>
#include <QScreen>

#include "gui/customwidgets/thumbnailwidget.h"
#include "gui/idirectoryview.h"
#include "shortcutbuilder.h"

enum ThumbnailViewOrientation {
    THUMBNAILVIEW_HORIZONTAL,
    THUMBNAILVIEW_VERTICAL
};

enum ThumbnailSelectMode {
    ACTIVATE_BY_PRESS,
    ACTIVATE_BY_DOUBLECLICK
};

class ThumbnailView : public QGraphicsView, public IDirectoryView {
    Q_OBJECT
public:
    ThumbnailView(ThumbnailViewOrientation orient, QWidget *parent = nullptr);
    virtual void setDirectoryPath(QString path) override;
    void select(QList<int>) override;
    void select(int) override;
    QList<int> selection() override;
    int itemCount();

    void setSelectMode(ThumbnailSelectMode mode);
    int lastSelected();
    void clearSelection();
    void deselect(int index);
public slots:
    void showEvent(QShowEvent *event) override;
    void resetViewport();
    int thumbnailSize();
    void loadVisibleThumbnails();
    void loadVisibleThumbnailsDelayed();

    void addItem();

    virtual void populate(int count) override;
    virtual void setThumbnail(int pos, std::shared_ptr<Thumbnail> thumb) override;
    virtual void insertItem(int index) override;
    virtual void removeItem(int index) override;
    virtual void reloadItem(int index) override;

    virtual void setDragHover(int index) override;

signals:
    void itemActivated(int) override;
    void thumbnailsRequested(QList<int>, int, bool, bool) override;
    void draggedOut() override;
    void draggedToBookmarks(QList<int>) override;
    void draggedOver(int) override;
    void droppedInto(const QMimeData*, QObject*, int) override;

private:
    ThumbnailViewOrientation orientation;

    QTimer loadTimer;
    bool blockThumbnailLoading;
    // let's say mouse wheel scroll always sends angleDelta = 120
    // then we can treat anything smaller as a touchpad scroll
    // TODO: tune this value
    const int SMOOTH_SCROLL_THRESHOLD = 120;

    int mDrawScrollbarIndicator, lastScrollFrameTime;
    QList<int> mSelection;

    bool mCropThumbnails, mouseReleaseSelect;
    ThumbnailSelectMode selectMode;
    QPoint dragStartPos;
    ThumbnailWidget* dragTarget;

    void createScrollTimeLine();
    QElapsedTimer scrollFrameTimer;
    std::function<void(int)> centerOn;

protected:
    QGraphicsScene scene;
    QList<ThumbnailWidget*> thumbnails;
    QScrollBar *scrollBar;
    QTimeLine *scrollTimeLine;
    QPointF viewportCenter;
    int mThumbnailSize;
    int offscreenPreloadArea = 3000;

    QList<int> rangeSelectionSnapshot;
    bool rangeSelection; // true if shift is pressed

    QRect indicator;
    const int indicatorSize = 2;

    int scrollRefreshRate = 16;
    const int SCROLL_DURATION = 120;
    const float SCROLL_MULTIPLIER = 2.5f;
    const float SCROLL_ACCELERATION = 1.4f;
    const int SCROLL_ACCELERATION_THRESHOLD = 50;

    const uint LOAD_DELAY = 150;

    bool atSceneStart();
    bool atSceneEnd();

    bool checkRange(int pos);

    virtual ThumbnailWidget *createThumbnailWidget() = 0;
    virtual void addItemToLayout(ThumbnailWidget* widget, int pos) = 0;
    virtual void removeItemFromLayout(int pos) = 0;
    virtual void removeAll() = 0;
    virtual void updateLayout();
    virtual void fitSceneToContents();
    virtual void ensureSelectedItemVisible() = 0;
    virtual void updateScrollbarIndicator() = 0;

    void setCropThumbnails(bool);
    void setDrawScrollbarIndicator(bool mode);

    void addSelectionRange(int indexTo);

    void wheelEvent(QWheelEvent *) override;
    void mousePressEvent(QMouseEvent *event) override;

    bool eventFilter(QObject *o, QEvent *ev) override;
    void resizeEvent(QResizeEvent *event);
    void scrollPrecise(int delta);
    void scrollSmooth(int delta);
    void scrollSmooth(int angleDelta, qreal multiplier, qreal acceleration);
    void scrollSmooth(int angleDelta, qreal multiplier, qreal acceleration, bool additive);
    void unloadAllThumbnails();
    void mouseDoubleClickEvent(QMouseEvent *event);
    void focusOutEvent(QFocusEvent *event) override;

    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void hideEvent(QHideEvent *event) override;
};
