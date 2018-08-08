/*
 * This panel will use sizeHint().height() from the widget it contains.
 * Width will match containerSize()Size.width()
 * TODO: implement "expand" switch.
 */

#include "slidehpanel.h"

SlideHPanel::SlideHPanel(OverlayContainerWidget *parent)
    : SlidePanel(parent)
{
    invisibleMargin = 12;
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
    if(position == PANEL_TOP)
        endPosition = startPosition - QPoint(0, slideAmount);
    else
        endPosition = QPoint(0, containerSize().height() - height() + slideAmount);
    recalculateGeometry();
}

void SlideHPanel::recalculateGeometry() {
    if(position == PANEL_TOP) {
        saveStaticGeometry(QRect(QPoint(0, 0),
                                 QPoint(containerSize().width() - 1, panelHeight - 1 + invisibleMargin)));
    } else {
        saveStaticGeometry(QRect(QPoint(0, containerSize().height() - panelHeight + 1),
                                 QPoint(containerSize().width(), containerSize().height()) - QPoint(0,1) ));
    }
    this->setGeometry(staticGeometry());
    updateTriggerRect();
}

void SlideHPanel::updateTriggerRect() {
    mTriggerRect = staticGeometry();
}
