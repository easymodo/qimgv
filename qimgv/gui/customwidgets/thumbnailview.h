#pragma once

/* This class manages QGraphicsScene, ThumbnailWidget list,
 * scrolling, requesting and setting thumbnails.
 * It doesn't do actual positioning of thumbnails within the scene.
 * (But maybe it should?)
 *
 * Usage: subclass, implement layout-related stuff
 */

#include <QGraphicsView>
#include <QOpenGLWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollBar>
#include <QWheelEvent>
#include <QTimeLine>
#include <QTimer>
#include <QElapsedTimer>
#include "gui/customwidgets/thumbnailwidget.h"
#include "gui/idirectoryview.h"

enum ThumbnailViewOrientation {
    THUMBNAILVIEW_HORIZONTAL,
    THUMBNAILVIEW_VERTICAL
};

enum ThumbnailSelectMode {
    SELECT_BY_PRESS,
    SELECT_BY_DOUBLECLICK
};

class ThumbnailView : public QGraphicsView, public IDirectoryView {
    Q_OBJECT
public:
    ThumbnailView(ThumbnailViewOrientation orient, QWidget *parent = nullptr);
    virtual void setDirectoryPath(QString path) Q_DECL_OVERRIDE;
    void selectIndex(int);
    int selectedIndex();
    int itemCount();

    void setSelectMode(ThumbnailSelectMode mode);
public slots:
    void showEvent(QShowEvent *event) Q_DECL_OVERRIDE;
    void resetViewport();
    int thumbnailSize();
    void loadVisibleThumbnails();
    void loadVisibleThumbnailsDelayed();

    void addItem();

    virtual void populate(int count) Q_DECL_OVERRIDE;
    virtual void setThumbnail(int pos, std::shared_ptr<Thumbnail> thumb) Q_DECL_OVERRIDE;
    virtual void insertItem(int index) Q_DECL_OVERRIDE;
    virtual void removeItem(int index) Q_DECL_OVERRIDE;
    virtual void reloadItem(int index) Q_DECL_OVERRIDE;

signals:
    void itemSelected(int) Q_DECL_OVERRIDE;
    void thumbnailsRequested(QList<int>, int, bool, bool) Q_DECL_OVERRIDE;
    void draggedOut(int index);

private:
    ThumbnailViewOrientation orientation;

    QTimer loadTimer;
    bool blockThumbnailLoading;
    // let's say mouse wheel scroll always sends angleDelta = 120
    // then we can treat anything smaller as a touchpad scroll
    // TODO: tune this value
    const int SMOOTH_SCROLL_THRESHOLD = 120;

    int mSelectedIndex, mDrawScrollbarIndicator, mDragTarget;

    bool mCropThumbnails;
    ThumbnailSelectMode selectMode;
    QPointF dragStartPos;
    ThumbnailWidget* dragTarget;

    void createScrollTimeLine();
protected:
    QGraphicsScene scene;
    QList<ThumbnailWidget*> thumbnails;
    QScrollBar *scrollBar;
    QTimeLine *scrollTimeLine;
    QPointF viewportCenter;
    int mThumbnailSize;
    int offscreenPreloadArea = 3000;

    QRect indicator;
    const int indicatorSize = 2;

    const int SCROLL_UPDATE_RATE = 7;
    const float SCROLL_SPEED_MULTIPLIER = 2.5f;
    const float SCROLL_SPEED_ACCELERATION = 1.5f;
    const int SCROLL_ANIMATION_SPEED = 120;

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

    void wheelEvent(QWheelEvent *) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

    bool eventFilter(QObject *o, QEvent *ev) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *event);
    void scrollPrecise(int delta);
    void scrollSmooth(int delta);
    void scrollSmooth(int angleDelta, qreal multiplier, qreal acceleration);
    void scrollSmooth(int angleDelta, qreal multiplier, qreal acceleration, bool additive);
    void unloadAllThumbnails();
    void mouseDoubleClickEvent(QMouseEvent *event);

    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
private slots:
    void centerOnX(int);
    void centerOnY(int);
};
