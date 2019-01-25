#ifndef CROPOVERLAY_H
#define CROPOVERLAY_H

#include "gui/customwidgets/overlaywidget.h"
#include <QDebug>
#include <QPainter>
#include <QPaintEvent>
#include <QColor>

enum CursorAction { NO_DRAG, DRAG_SELECT, DRAG_MOVE, DRAG_LEFT, DRAG_RIGHT,
                        DRAG_TOP, DRAG_BOTTOM, DRAG_TOPLEFT,
                        DRAG_TOPRIGHT, DRAG_BOTTOMLEFT, DRAG_BOTTOMRIGHT };

class CropOverlay : public OverlayWidget
{
    Q_OBJECT
public:
    explicit CropOverlay(OverlayContainerWidget *parent = nullptr);
    void setImageDrawRect(QRect);
    void setImageRealSize(QSize);
    void setButtonText(QString text);
    void setImageScale(float scale);
    void clearSelection();

signals:
    void positionChanged(float x, float y);
    void selectionChanged(QRect);
    void escPressed();
    void enterPressed();

protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);

    void keyPressEvent(QKeyEvent *event);
private:
    QWidget *viewer;
    QPoint startPos, endPos, moveStartPos, resizeAnchor;
    QRect imageRect, imageDrawRect, selectionRect, selectionDrawRect, handles[8];
    bool clear, moving, forceAspectRatio;
    float scale;
    QBrush brushInactiveTint, brushDarkGray, brushGray, brushLightGray;
    QRectF selectionDrawRectDpi, handlesDpi[8];
    int handleSize;
    QImage *drawBuffer;
    CursorAction cursorAction;
    QPen selectionOutlinePen;
    qreal dpr;
    QPointF ar;

    QPoint setInsidePoint(QPoint, QRect);
    QRect placeInside(QRect what, QRect where);
    void drawSelection(QPainter*);
    void drawHandles(QBrush&, QPainter*);
    void updateHandlePositions();
    void prepareDrawElements();
    CursorAction hoverTarget(QPoint pos);
    void resizeSelection(QPoint d);
    void resizeSelectionAR2(QPoint d);
    void resizeSelectionFree(QPoint d);
    void recalculateGeometry();
    QPoint mapPointToImage(QPoint p);
    void updateSelectionDrawRect();
    void setCursorAction(CursorAction action);
    void setResizeAnchor(CursorAction action);
public slots:
    void show();
    void hide();
    void onSelectionOutsideChange(QRect selection);
    void selectAll();
    void setAspectRatio(QPointF);
    void setForceAspectRatio(bool mode);
};

#endif // CROPOVERLAY_H
