/* Base class for floating widgets.
 * It will automatically reposition itself according to FloatingWidgetPosition.
 */

#pragma once

#include "gui/customwidgets/floatingwidget.h"
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

class OverlayWidget : public FloatingWidget
{
    Q_OBJECT
    Q_PROPERTY (qreal opacity READ opacity WRITE setOpacity)
public:
    OverlayWidget(FloatingWidgetContainer *parent);
    ~OverlayWidget();
    void setHorizontalMargin(int);
    void setVerticalMargin(int);
    int horizontalMargin();
    int verticalMargin();
    void setPosition(FloatingWidgetPosition pos);
    void setFadeDuration(int duration);
    void setFadeEnabled(bool mode);

public slots:
    void show();
    void hide();
    void hideAnimated();

private:
    QGraphicsOpacityEffect *opacityEffect;
    int mHorizontalMargin, mVerticalMargin;
    bool fadeEnabled;
    QPropertyAnimation *fadeAnimation;

private slots:
    void setOpacity(qreal opacity);
    qreal opacity() const;

protected:
    virtual void recalculateGeometry();
    FloatingWidgetPosition position;
};
