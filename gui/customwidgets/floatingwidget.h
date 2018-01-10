/* Base class for floating widgets.
 * It will automatically reposition itself according to FloatingWidgetPosition.
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
    FloatingWidget(ContainerWidget *parent);
    void setMarginX(int);
    void setMarginY(int);
    void setPosition(FloatingWidgetPosition pos);

private:
    int marginX, marginY;

protected:
    virtual void recalculateGeometry();
    FloatingWidgetPosition position;

};

#endif // FLOATINGWIDGET_H
