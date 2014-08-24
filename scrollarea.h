#ifndef ZZSCROLLAREA_H
#define ZZSCROLLAREA_H

#include <QScrollArea>
#include <QScrollBar>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QKeyEvent>
#include <QPoint>
#include <QDebug>
#include <QSize>
#include "imageviewer.h"

class ScrollArea : public QScrollArea
{
    Q_OBJECT
public:
    explicit ScrollArea(QWidget *parent = 0);
    int getAspect() const;
    void setImagePath(const QString& path);
    void fitImageHorizontal();
    void fitImageVertical();
    void fitImageDefault();
    void fitImageOriginal();
    void scaleImage(double factor);
    
signals:
    void sendDoubleClick();
    void resized();
    void scrollbarChanged();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void resizeEvent(QResizeEvent *event);
    void keyPressEvent(QKeyEvent *event);

private:
    QPoint lastDragPosition;
    QScrollBar *mHBar, *mVBar;
    QSize mSize;
    int mAspect;
    ImageViewer* mImageViewer;
};

#endif // ZZSCROLLAREA_H
