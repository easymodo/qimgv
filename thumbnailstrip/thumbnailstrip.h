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
#include "thumbnaillabel.h"

class ThumbnailStrip : public QWidget
{
    Q_OBJECT
public:
    explicit ThumbnailStrip(QWidget *_parent = 0);
    ~ThumbnailStrip();
    QList<ThumbnailLabel*> thumbnailLabels;

    QSize sizeHint() const;
private:
    QWidget *parent;
    void addItem(int pos);
    QHBoxLayout *layout;

    bool visible(int pos);
signals:
    void thumbnailClicked(int);

public slots:
    void populate(int count);

protected:
    void wheelEvent(QWheelEvent *event);

private slots:
    void slotThumbnailClicked(ThumbnailLabel*);
};

#endif // THUMBNAILSTRIP_H
