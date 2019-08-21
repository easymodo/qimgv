/*
 * This panel will use sizeHint().height() from the widget it contains.
 * Width will match containerSize()Size.width()
 * TODO: implement "expand" switch.
 */

#include "slidehpanel.h"

SlideHPanel::SlideHPanel(OverlayContainerWidget *parent)
    : SlidePanel(parent)
{
    bottomMargin = 6;
    panelHeight = 100;
    mLayout.setContentsMargins(0,0,0,0);
    setPosition(PANEL_TOP);
}

SlideHPanel::~SlideHPanel() {

}

QRect SlideHPanel::triggerRect() {
    return mTriggerRect;
}

void SlideHPanel::setPanelHeight(int newHeight) {
    if(newHeight != panelHeight) {
        panelHeight = newHeight;
        recalculateGeometry();
    }
}

void SlideHPanel::setPosition(PanelHPosition p) {
    position = p;
    recalculateGeometry();
}

void SlideHPanel::recalculateGeometry() {
    if(position == PANEL_TOP) {
        setAnimationRange(QPoint(0,0),
                          QPoint(0,0) - QPoint(0, slideAmount));
        saveStaticGeometry(QRect(QPoint(0, 0),
                                 QPoint(containerSize().width() - 1, panelHeight - 1 + bottomMargin)));
    } else {
        setAnimationRange(QPoint(0, containerSize().height() - height()),
                          QPoint(0, containerSize().height() - height() + slideAmount));
        saveStaticGeometry(QRect(QPoint(0, containerSize().height() - panelHeight + 1),
                                 QPoint(containerSize().width(), containerSize().height()) - QPoint(0,1) ));
    }
    this->setGeometry(staticGeometry());
    updateTriggerRect();
}

void SlideHPanel::updateTriggerRect() {
    mTriggerRect = staticGeometry();
}
