#ifndef THUMBNAILSTRIP_H
#define THUMBNAILSTRIP_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QScrollBar>
#include "../imagecache.h"
#include "customscene.h"

class ThumbnailStrip : public QGraphicsView
{
    Q_OBJECT
public:
    explicit ThumbnailStrip(const ImageCache *_cache, QWidget *parent = 0);
    ~ThumbnailStrip();

private:
    const ImageCache *cache;
    CustomScene* scene;
    void addItem(int pos);

signals:
    void thumbnailClicked(int);

public slots:
    void populate();

protected:
    void wheelEvent(QWheelEvent *event);

private slots:
    void sceneClicked(int wPos);
};

#endif // THUMBNAILSTRIP_H
