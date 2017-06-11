#include "controlsoverlay.h"

ControlsOverlay::ControlsOverlay(QWidget *parent) :
    OverlayWidget(parent)
{
    settingsButton = new IconButton("openSettings", ":/images/res/icons/settings.png");
    closeButton = new IconButton("exit", ":/images/res/icons/close.png");
    layout.setContentsMargins(0,0,0,0);
    this->setContentsMargins(0,0,0,0);
    layout.setSpacing(0);
    layout.addWidget(settingsButton);
    layout.addWidget(closeButton);
    setLayout(&layout);
    fitToContents();
}

QSize ControlsOverlay::contentsSize() {
    QSize newSize(0, 0);
    for(int i=0; i<layout.count(); i++) {
        newSize.setWidth(newSize.width() + layout.itemAt(i)->widget()->width());
        newSize.setHeight(layout.itemAt(i)->widget()->height());
    }
    return newSize;
}

void ControlsOverlay::fitToContents() {
    this->setFixedSize(contentsSize());
    recalculateGeometry();
}

void ControlsOverlay::recalculateGeometry() {
    setGeometry(containerSize().width() - width(), 0, width(), height());
}
