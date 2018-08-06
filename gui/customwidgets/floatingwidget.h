/* Base class for floating widgets.
 * It will automatically reposition itself according to FloatingWidgetPosition.
 */

#ifndef FLOATINGWIDGET_H
#define FLOATINGWIDGET_H

#include "gui/customwidgets/overlaywidget.h"
#include <QTimeLine>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QDebug>

enum FloatingWidgetPosition {
    LEFT,
    RIGHT,
    BOTTOM,
    TOP,
    TOPLEFT,
    TOPRIGHT,
    BOTTOMLEFT,
    BOTTOMRIGHT,
    CENTER
};

class FloatingWidget : public OverlayWidget
{
    Q_OBJECT
    Q_PROPERTY (qreal opacity READ opacity WRITE setOpacity)
public:
    FloatingWidget(OverlayContainerWidget *parent);
    ~FloatingWidget();
    void setMarginX(int);
    void setMarginY(int);
    void setPosition(FloatingWidgetPosition pos);
    void setFadeDuration(int duration);
    void setFadeEnabled(bool mode);

public slots:
    void show();
    void hide();

private:
    QGraphicsOpacityEffect *opacityEffect;
    int marginX, marginY;
    bool fadeEnabled;
    QPropertyAnimation *fadeAnimation;

private slots:
    void setOpacity(qreal opacity);
    qreal opacity() const;

    void onFadeEnd();
protected:
    virtual void recalculateGeometry();
    FloatingWidgetPosition position;

};

#endif // FLOATINGWIDGET_H
