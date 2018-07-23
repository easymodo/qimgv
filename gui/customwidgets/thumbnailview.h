#ifndef THUMBNAILVIEW_H
#define THUMBNAILVIEW_H

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

#include "gui/customwidgets/thumbnailwidget.h"

enum ThumbnailViewOrientation {
    THUMBNAILVIEW_HORIZONTAL,
    THUMBNAILVIEW_VERTICAL
};

class ThumbnailView : public QGraphicsView {
    Q_OBJECT
public:
    ThumbnailView(ThumbnailViewOrientation orient, QWidget *parent = nullptr);

public slots:
    void showEvent(QShowEvent *event);
    void populate(int count);
    void setThumbnail(int pos, std::shared_ptr<Thumbnail> thumb);
    void resetViewport();
    void loadVisibleThumbnails();

signals:
    void scrolled();
    void thumbnailPressed(int);
    void thumbnailRequested(QList<int>, int);

private:
    ThumbnailViewOrientation orientation;

    void scrollPrecise(int pixelDelta);
    void scrollSmooth(int angleDelta);

protected:
    QGraphicsScene scene;
    QList<ThumbnailWidget*> thumbnails;
    QScrollBar *scrollBar;
    QTimeLine *timeLine;
    QTimer scrollTimer;
    QPointF viewportCenter;
    int thumbnailSize;

    const int SCROLL_UPDATE_RATE = 8;
    const float SCROLL_SPEED_MULTIPLIER = 4.0f;
    const int SCROLL_ANIMATION_SPEED = 140;

    const uint LOAD_DELAY = 40;
    const int OFFSCREEN_PRELOAD_AREA = 3000;

    bool atSceneStart();
    bool atSceneEnd();

    bool checkRange(int pos);

    virtual ThumbnailWidget *createThumbnailWidget() = 0;
    virtual void addItemToLayout(ThumbnailWidget* widget, int pos) = 0;
    virtual void removeItemFromLayout(int pos) = 0;
    virtual void onPopulate();
    void fitSceneToContents();
    virtual void ensureSelectedItemVisible() = 0;

    void wheelEvent(QWheelEvent *) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event);

private slots:
    void centerOnX(int);
    void centerOnY(int);

};

#endif // THUMBNAILVIEW_H
