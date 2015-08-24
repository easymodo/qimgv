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

class ThumbnailStrip : public QGraphicsView
{
    Q_OBJECT
public:
    explicit ThumbnailStrip(QWidget *parent = 0);
    ~ThumbnailStrip();
    QList<ThumbnailLabel*> thumbnailLabels;

private:
    void addItem();
    int itemCount;
    QGraphicsLinearLayout *layout;
    QGraphicsWidget *widget;
    CustomScene* scene;

    const qreal OPACITY_INACTIVE = 0.65;
    const qreal OPACITY_SELECTED = 1.0;
    const int ANIMATION_SPEED_INSTANT = 0;
    const int ANIMATION_SPEED_FAST = 100;
    const int ANIMATION_SPEED_NORMAL = 220;

    int scrollStep;
    int defaultHeight;
    bool childVisible(int pos);
    ThumbnailStrip *strip;
    QRectF visibleRegion;
    QTimer loadTimer;
    uint loadDelay;
    int previous;
    int offscreenPreloadArea;
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
    void setThumbnail(int, const QPixmap*);
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
