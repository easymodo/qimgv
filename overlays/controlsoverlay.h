#ifndef ControlsOverlay_H
#define ControlsOverlay_H
#include "../customWidgets/overlaywidget.h"
#include "../customWidgets/iconbutton.h"
#include <QHBoxLayout>
#include <QDebug>

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
