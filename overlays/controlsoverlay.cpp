#include "controlsoverlay.h"

ControlsOverlay::ControlsOverlay(QWidget *parent) :
    ClickableLabel(parent)
{
    setPalette(Qt::transparent);
    setAccessibleName("panelButtonTransparent");
    setPixmap(QPixmap(":/images/res/icons/window-close-transparent.png"));
    this->setContentsMargins(0,0,0,0);
    connect(this, SIGNAL(clicked()), this, SIGNAL(exitClicked()));
}

void ControlsOverlay::updatePosition(QSize containerSz) {
    this->setGeometry(containerSz.width() - pixmap()->width() - 5,
                      0,
                      pixmap()->width() + 5,
                      pixmap()->height() + 6);
}
