#ifndef THUMBNAILVIEW_H
#define THUMBNAILVIEW_H

#include <QFrame>
#include <QGraphicsView>
#include <QHBoxLayout>
#include <QWheelEvent>
#include <QScrollBar>
#include <QTimeLine>
#include <QTimer>
#include <QDebug>
#include "thumbnaillabel.h"

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
    QTimer scrollTimer;
    QPointF viewportCenter;
    bool forceSmoothScroll;

    const int SCROLL_UPDATE_RATE = 8; // 16 feels too laggy
    const float SCROLL_SPEED_MULTIPLIER = 3.6;
    const int SCROLL_ANIMATION_SPEED = 180;

    bool atSceneStart();
    bool atSceneEnd();
private slots:
    void centerOnX(int);
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
