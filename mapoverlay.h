#ifndef MAPOVERLAY_H
#define MAPOVERLAY_H

#include <QWidget>
#include <QPainter>
#include <QPen>
#include <QDebug>
#include <QPaintEvent>

class MapOverlayPrivate;

class MapOverlay : public QWidget
{
    Q_OBJECT
public:
    explicit MapOverlay(QWidget *parent = 0);
    
    /**
     * @brief Updating navigation map
     * 
     * calculates outer(image) and inner(view area) squares.
     * 
     * @param windowSz window size
     * @param drawingRect drawing area size
     */
    
    void updateMap(const QSize& windowRect, const QRect& drawingRect);

protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    
private:
    MapOverlayPrivate* d;
};

#endif // MAPOVERLAY_H
