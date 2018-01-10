/* Base class for floating widgets.
 * It will uatomatically reposition itself according to FloatingWidgetPosition.
 * Usage: implement calculateDesiredSize() method.
 * Call setContainerSize(QSize) whenever its container resized.
 */

#ifndef FLOATINGWIDGET_H
#define FLOATINGWIDGET_H

#include "gui/customwidgets/overlaywidget.h"
#include <QDebug>

enum FloatingWidgetPosition {
    LEFT,
    RIGHT,
    BOTTOM,
    TOP,
    TOPLEFT,
    TOPRIGHT,
    BOTTOMLEFT,
    BOTTOMRIGHT
};

class FloatingWidget : public OverlayWidget
{
    Q_OBJECT
public:
    FloatingWidget(QWidget *parent = nullptr);
    void setMarginX(int);
    void setMarginY(int);

private:
    int marginX, marginY;

protected:
    virtual void recalculateGeometry();
    FloatingWidgetPosition position;

};

#endif // FLOATINGWIDGET_H
