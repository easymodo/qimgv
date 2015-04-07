#ifndef CROPOVERLAY_H
#define CROPOVERLAY_H

#include <QWidget>
#include <QDebug>
#include <QPainter>
#include <QPaintEvent>
#include <QColor>
#include <QFont>

class CropOverlay : public QWidget
{
    Q_OBJECT
public:
    explicit CropOverlay(QWidget *parent = 0);
    void setImageArea(QRect);
    void setRealSize(QSize);

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
    QSize realSize;
    bool clear, moving;
    float scale;
    QBrush brushDark;
    QBrush brushGray;

    QPoint setInsidePoint(QPoint, QRect);
    void clearSelection();
    QRect mapSelection();
    void drawLabel(QString text, QPoint pos, QPainter*);
    void drawAroundSelection(QPainter *);
public slots:
    void setImageArea(QRect area, float _scale);
    void hide();
    void display();

signals:
    void cropSelected(QRect);
};

#endif // CROPOVERLAY_H
