#ifndef ControlsOverlay_H
#define ControlsOverlay_H

#include <QHBoxLayout>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QDebug>
#include "gui/customwidgets/overlaywidget.h"
#include "gui/customwidgets/actionbutton.h"

class ControlsOverlay : public OverlayWidget
{
    Q_OBJECT
public:
    explicit ControlsOverlay(OverlayContainerWidget *parent);

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
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);
};

#endif // ControlsOverlay_H
