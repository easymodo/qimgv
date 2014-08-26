#ifndef SCROLLAREA_H
#define SCROLLAREA_H

#include <QScrollArea>
#include <QScrollBar>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QKeyEvent>
#include <QPoint>
#include <QDebug>
#include <QSize>
#include "customlabel.h"
#include "image.h"

class ScrollArea : public QScrollArea
{
    Q_OBJECT
public:
    explicit ScrollArea(QWidget *parent = 0);
    double getAspect() const;
    void displayImage(Image*);
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
    QScrollBar *hBar, *vBar;
    QSize mSize;
    int mAspect;
    CustomLabel *label;
    Image *currentImage;
};

#endif // SCROLLAREA_H
