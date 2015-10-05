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
#include <QTimeLine>
#include <QPropertyAnimation>
#include "../customWidgets/customscene.h"
#include "../sourceContainers/thumbnail.h"
#include "thumbnaillabel.h"

class ThumbnailStrip : public QGraphicsView
{
    Q_OBJECT
public:
    explicit ThumbnailStrip(QWidget *parent = 0);
    ~ThumbnailStrip();

private:
    QList<ThumbnailLabel*> thumbnailLabels;
    void addItem();
    QGraphicsLinearLayout *layout;
    QGraphicsWidget *widget;
    CustomScene* scene;
    QTimeLine *timeLine;

    const qreal OPACITY_INACTIVE = 0.75;
    const qreal OPACITY_SELECTED = 1.0;
    const int ANIMATION_SPEED_INSTANT = 0;
    const int ANIMATION_SPEED_FAST = 100;
    const int ANIMATION_SPEED_NORMAL = 220;

    const int SCROLL_UPDATE_RATE = 8;
    const float SCROLL_SPEED_MULTIPLIER = 3.1;
    const int SCROLL_ANIMATION_SPEED = 80;
    const int SCROLL_STEP = 200;
    const uint LOAD_DELAY = 20;
    const int OFFSCREEN_PRELOAD_AREA = 1900;

    int panelSize;
    int itemCount, current, thumbnailSize;
    bool childVisible(int pos);
    ThumbnailStrip *strip;
    QRectF preloadArea, visibleRegion;
    QTimer loadTimer;
    bool childVisibleEntirely(int pos);
    QRectF itemsBoundingRect();
    QScrollBar *scrollBar;
    PanelPosition position;

    void requestThumbnailLoad(int pos);
signals:
    void thumbnailRequested(int pos);
    void thumbnailClicked(int pos);

public slots:
    void populate(int count);
    void parentResized(QSize parentSize);
    void loadVisibleThumbnails();
    void loadVisibleThumbnailsDelayed();
    void setThumbnail(int, Thumbnail*);
    void fillPanel(int);
    void selectThumbnail(int pos);

protected:
    void wheelEvent(QWheelEvent *event);
    void leaveEvent(QEvent *event);

private slots:
    void centerOnSmooth(const QPointF &pos);
    void sceneClicked(QPointF pos);
    void updateVisibleRegion();
    void translateX(int dx);
    void readSettings();
};

#endif // THUMBNAILSTRIP_H
