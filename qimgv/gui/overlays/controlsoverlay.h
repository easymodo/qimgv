#pragma once

#include <QHBoxLayout>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QDebug>
#include "gui/customwidgets/floatingwidget.h"
#include "gui/customwidgets/actionbutton.h"

class ControlsOverlay : public FloatingWidget
{
    Q_OBJECT
public:
    explicit ControlsOverlay(FloatingWidgetContainer *parent);

public slots:
    void show();

private:
    QHBoxLayout layout;
    ActionButton *closeButton, *settingsButton, *folderViewButton;
    QGraphicsOpacityEffect *fadeEffect;
    QPropertyAnimation *fadeAnimation;
    QSize contentsSize();
    void fitToContents();

protected:
    virtual void recalculateGeometry();
#if QT_VERSION > QT_VERSION_CHECK(6,0,0)
    void enterEvent(QEnterEvent *event);
#else
    void enterEvent(QEvent *event);
#endif
    void leaveEvent(QEvent *event);
};
