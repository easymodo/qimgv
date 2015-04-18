#ifndef THUMBNAILSCROLLAREA_H
#define THUMBNAILSCROLLAREA_H

#include <QScrollArea>
#include <QScrollBar>
#include <QWheelEvent>

class ThumbnailScrollArea : public QScrollArea
{
public:
    ThumbnailScrollArea();
    ~ThumbnailScrollArea();

private:
    int scrollStep;
protected:
    void wheelEvent(QWheelEvent*);
};

#endif // THUMBNAILSCROLLAREA_H
