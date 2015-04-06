#ifndef MAPOVERLAY_H
#define MAPOVERLAY_H

#include <QWidget>
#include <QPainter>
#include <QPen>
#include <QDebug>
#include <QPaintEvent>

class MapOverlay : public QWidget
{
    Q_OBJECT
    Q_PROPERTY (float opacity READ opacity WRITE setOpacity);
public:
    enum Location 
    {
        LeftTop,
        RightTop,
        RightBottom,
        LeftBottom
    };
    
    explicit MapOverlay(QWidget *parent = 0);
    virtual ~MapOverlay();
    void resize(int size);
    int size() const;
    void setAutoVisible(bool state = true);
    void animateVisible(bool visible);
    
    void setOpacity(float opacity);
    float opacity() const;
    
    void setLocation(Location l);
    Location location() const;
    
    void setMargin(int margin);
    int margin() const;
    
    /**
     * @brief Updating navigation map
     * calculates outer(image) and inner(view area) squares.
     */
    void updateMap(const QRectF& windowRect, const QRectF& drawingRect);
    
    /**
     * Recalculates map position on resize
     */
    void parentResized(int width, int height);
    
    /**
     * Unit test functions
     */
    QSizeF inner() const;
    QSizeF outer() const;
signals:
    void positionChanged(float x, float y);
    
protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    
private:
    class MapOverlayPrivate;
    MapOverlayPrivate* d;
};

#endif // MAPOVERLAY_H
