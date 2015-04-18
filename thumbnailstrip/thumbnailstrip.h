#ifndef THUMBNAILSTRIP_H
#define THUMBNAILSTRIP_H

#include <QWidget>
#include <QLabel>
#include <QLayout>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QScrollBar>
#include "../imagecache.h"
#include "thumbnaillabel.h"

class ThumbnailStrip : public QWidget
{
    Q_OBJECT
public:
    explicit ThumbnailStrip(ImageCache* _cache, QWidget *parent = 0);
    ~ThumbnailStrip();

private:
    const ImageCache *cache;
    void addItem(int pos);
    QHBoxLayout *layout;
    QList<QLabel*> thumbnailLabels;

signals:
    void thumbnailClicked(int);

public slots:
    void populate();

protected:
    void wheelEvent(QWheelEvent *event);

private slots:
    void slotThumbnailClicked(QLabel*);
};

#endif // THUMBNAILSTRIP_H
