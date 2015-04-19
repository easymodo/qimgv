#ifndef THUMBNAILSCROLLAREA_H
#define THUMBNAILSCROLLAREA_H

#include <QWidget>
#include <QMouseEvent>
#include <QScrollArea>
#include <QScrollBar>
#include <QWheelEvent>
#include <QDebug>

class ThumbnailScrollArea : public QScrollArea
{
    Q_OBJECT
public:
    ThumbnailScrollArea(QWidget *parent = 0);
    ~ThumbnailScrollArea();

public slots:
    void parentResized(QSize parentSize);

private:
    int scrollStep;
    int defaultHeight;

protected:
    void wheelEvent(QWheelEvent*);
    void leaveEvent(QEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
};

#endif // THUMBNAILSCROLLAREA_H
