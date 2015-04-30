#ifndef THUMBNAILSCROLLAREA_H
#define THUMBNAILSCROLLAREA_H

#include <QWidget>
#include <QMouseEvent>
#include <QScrollArea>
#include <QScrollBar>
#include <QWheelEvent>
#include <QDebug>
#include "thumbnailstrip.h"

class ThumbnailScrollArea : public QScrollArea
{
    Q_OBJECT
public:
    ThumbnailScrollArea(QWidget *parent = 0);
    ~ThumbnailScrollArea();

public slots:
    void parentResized(QSize parentSize);
    void loadVisibleThumbnails();   
    void setThumbnail(int, const QPixmap*);
    void cacheInitialized(int);

signals:
    void thumbnailRequested(int pos);
    void thumbnailClicked(int pos);

private:
    int scrollStep;
    int defaultHeight;
    bool childVisible(ThumbnailLabel *label);
    ThumbnailStrip *strip;

protected:
    void wheelEvent(QWheelEvent*);
    void leaveEvent(QEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
};

#endif // THUMBNAILSCROLLAREA_H
