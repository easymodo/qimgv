#ifndef THUMBNAILVIEW_H
#define THUMBNAILVIEW_H

#include <QFrame>
#include <QGraphicsView>
#include "thumbnaillabel.h"
#include <QHBoxLayout>
#include <QWheelEvent>
#include <QScrollBar>
#include <QTimeLine>
#include <QDebug>

class ThumbnailFrame;

class GraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    GraphicsView(ThumbnailFrame *v);
    void resetViewport();

protected:
#ifndef QT_NO_WHEELEVENT
    void wheelEvent(QWheelEvent *) Q_DECL_OVERRIDE;
#endif
    void mousePressEvent(QMouseEvent *event);
private:
    QScrollBar *scrollBar;
    ThumbnailFrame *frame;
    QTimeLine *timeLine;
    QPointF viewportCenter;
    bool forceSmoothScroll;

    const int SCROLL_UPDATE_RATE = 8;
    const float SCROLL_SPEED_MULTIPLIER = 3.4;
    const int SCROLL_ANIMATION_SPEED = 180;

private slots:
    void doScroll(int);
    void readSettings();

signals:
    void scrolled();
};

class ThumbnailFrame : public QFrame
{
    Q_OBJECT
public:
    explicit ThumbnailFrame(QWidget *parent = 0);

    GraphicsView *view() const;
    void addItem(QGraphicsItem* item);
    void acceptThumbnailClick(int);

signals:
    void thumbnailClicked(int);
    void scrolled();

private:
    GraphicsView *graphicsView;
};

#endif // THUMBNAILVIEW_H
