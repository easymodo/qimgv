#ifndef THUMBNAILVIEW_H
#define THUMBNAILVIEW_H

#include <QScrollArea>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QDebug>

class ThumbnailView : public QScrollArea
{
public:
    ThumbnailView();

protected:
    void wheelEvent(QWheelEvent *event);
    void mousePressEvent(QMouseEvent *event);
};

#endif // THUMBNAILVIEW_H
