#ifndef THUMBNAILSTRIP_H
#define THUMBNAILSTRIP_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QGraphicsWidget>
#include <QGraphicsLinearLayout>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QScrollBar>
#include <QTimer>
#include <QPropertyAnimation>
#include "customscene.h"
#include "thumbnaillabel.h"
#include "thumbnail.h"

class ThumbnailStrip : public QGraphicsView
{
    Q_OBJECT
public:
    explicit ThumbnailStrip(QWidget *parent = 0);
    ~ThumbnailStrip();
    QList<ThumbnailLabel*> thumbnailLabels;

private:
    void addItem();
    QGraphicsLinearLayout *layout;
    QGraphicsWidget *widget;
    CustomScene* scene;

    const qreal OPACITY_INACTIVE = 0.65;
    const qreal OPACITY_SELECTED = 1.0;
    const int ANIMATION_SPEED_INSTANT = 0;
    const int ANIMATION_SPEED_FAST = 100;
    const int ANIMATION_SPEED_NORMAL = 220;

    const int SCROLL_STEP = 200;
    const int DEFAULT_HEIGHT = 124;
    const uint LOAD_DELAY = 30;
    const int OFFSCREEN_PRELOAD_AREA = 400;

    int itemCount;
    int current;
    bool childVisible(int pos);
    ThumbnailStrip *strip;
    QRectF visibleRegion;
    QTimer loadTimer;
    bool childVisibleEntirely(int pos);
    QRectF itemsBoundingRect();

signals:
    void thumbnailRequested(int pos);
    void thumbnailClicked(int pos);

public slots:
    void populate(int count);
    void parentResized(QSize parentSize);
    void loadVisibleThumbnails();
    void loadVisibleThumbnailsDelayed();
    void setThumbnail(int, const Thumbnail*);
    void fillPanel(int);
    void selectThumbnail(int pos);

protected:
    void wheelEvent(QWheelEvent *event);
    void leaveEvent(QEvent *event);

private slots:
    void sceneClicked(QPointF pos);
    void updateVisibleRegion();
};

#endif // THUMBNAILSTRIP_H
