#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <QScrollArea>
#include <QScrollBar>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QKeyEvent>
#include <QPoint>
#include <QColor>
#include <QDebug>
#include "customlabel.h"
#include "image.h"

enum FitMode { NORMAL, WIDTH, ALL };

class ImageViewer : public QScrollArea
{
    Q_OBJECT
    public:
        explicit ImageViewer(QWidget *parent = 0);
        void displayImage(Image*);
        void refit();

    signals:
        void sendDoubleClick();
        void resized();
        void scrollbarChanged();

    public slots:
        void fitAll();
        void fitWidth();
        void fitNormal();

    protected:
        void mousePressEvent(QMouseEvent *event);
        void mouseReleaseEvent(QMouseEvent *event);
        void mouseMoveEvent(QMouseEvent *event);
        void wheelEvent(QWheelEvent *event);
        void mouseDoubleClickEvent(QMouseEvent *event);
        void resizeEvent(QResizeEvent *event);
        void keyPressEvent(QKeyEvent *event);
        QPoint lastDragPos,temp;
    private:
        CustomLabel *label;
        QScrollBar *vBar, *hBar;
        Image *img;
        FitMode fitMode;
        void fitDefault();
};

#endif // IMAGEVIEWER_H
