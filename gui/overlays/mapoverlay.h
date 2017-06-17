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
    Q_PROPERTY (float opacity READ opacity WRITE setOpacity)
    Q_PROPERTY (int y READ y WRITE setY)
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
    void animateVisible(bool visible);
    
    void setOpacity(float opacity);
    float opacity() const;

    void enableVisibility(bool);
    
    void setLocation(Location l);
    Location location() const;
    
    void setMargin(int margin);
    int margin() const;
    
    void setY(int y);
    int y() const;
    
    /**
     * @brief Updating navigation map
     * calculates outer(image) and inner(view area) squares.
     */
    void updateMap(const QRectF& drawingRect);
    
    /**
     * Recalculates map position on resize
     */
    void updatePosition();
    
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
    virtual void resizeEvent(QResizeEvent* event);
    virtual void leaveEvent(QEvent *);
    virtual void enterEvent(QEvent *);
    
private:
    bool visibilityEnabled, imageDoesNotFit;
    class MapOverlayPrivate;
    MapOverlayPrivate* d;
};

#endif // MAPOVERLAY_H
