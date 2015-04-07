#ifndef CROPOVERLAY_H
#define CROPOVERLAY_H

#include <QWidget>
#include <QDebug>
#include <QPainter>
#include <QPaintEvent>
#include <QColor>

class CropOverlay : public QWidget
{
    Q_OBJECT
public:
    explicit CropOverlay(QWidget *parent = 0);
    void setImageArea(QRect);

    QRect placeInside(QRect what, QRect where);
signals:
    void positionChanged(float x, float y);

protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void keyPressEvent(QKeyEvent *event);

private:
    QWidget *viewer;
    QPoint startPos, endPos, moveStartPos;
    QRect imageArea, selectionRect;
    bool clear, moving;

    QPoint setInsidePoint(QPoint, QRect);
    void clearSelection();
public slots:
    void hide();
    void display();
};

#endif // CROPOVERLAY_H
