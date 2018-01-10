#ifndef CROPOVERLAY_H
#define CROPOVERLAY_H

#include "gui/customwidgets/overlaywidget.h"
#include <QDebug>
#include <QPainter>
#include <QPaintEvent>
#include <QColor>

enum MouseDragMode { NO_DRAG, MOVE, DRAG_LEFT, DRAG_RIGHT,
                        DRAG_TOP, DRAG_BOTTOM, DRAG_TOPLEFT,
                        DRAG_TOPRIGHT, DRAG_BOTTOMLEFT, DRAG_BOTTOMRIGHT };

class CropOverlay : public OverlayWidget
{
    Q_OBJECT
public:
    explicit CropOverlay(ContainerWidget *parent = 0);
    void setImageRect(QRectF);
    void setImageRealSize(QSize);
    void setButtonText(QString text);
    void setImageScale(float scale);

signals:
    void positionChanged(float x, float y);
    void selected(QRect);
    void selectionChanged(QRect);

protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void keyPressEvent(QKeyEvent *event);

private:
    QWidget *viewer;
    QPointF startPos, endPos, moveStartPos;
    QRectF imageRect, selectionRect;
    QSize imageRealSize;
    bool clear, moving;
    float scale;
    QBrush brushInactiveTint, brushDarkGray, brushGray, brushLightGray;
    QRectF *handles[8];
    int handleSize;
    QImage *drawBuffer;
    MouseDragMode dragMode;
    QPen selectionOutlinePen;

    QPointF setInsidePoint(QPoint, QRectF);
    QRectF placeInside(QRectF what, QRectF where);
    void clearSelection();
    void selectAll();
    QRect mapSelection();
    void drawSelection(QPainter *);
    void drawHandles(QBrush&, QPainter*);
    void updateHandlePositions();
    void prepareDrawElements();
    void detectClickTarget(QPoint pos);
    bool resizeSelection(QPointF d);
    void onSelectionChanged();
    void recalculateGeometry();
public slots:
    //void setImageRect(QRect area, float _scale);
    void hide();
    void show();
    void onSelectionOutsideChange(QRect unmapped);
};

#endif // CROPOVERLAY_H
