#ifndef ControlsOverlay_H
#define ControlsOverlay_H

#include <QHBoxLayout>
#include <QDebug>
#include "gui/customwidgets/overlaywidget.h"
#include "gui/customwidgets/iconbutton.h"

class ControlsOverlay : public OverlayWidget
{
    Q_OBJECT
public:
    explicit ControlsOverlay(QWidget *parent);

private:
    QHBoxLayout layout;
    IconButton *closeButton, *settingsButton;
    QSize contentsSize();
    void fitToContents();

protected:
    virtual void recalculateGeometry();
};

#endif // ControlsOverlay_H
