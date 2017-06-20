/*
 * This panel will use sizeHint().height() from the widget it contains.
 * Width will match containerSize()Size.width()
 * TODO: implement "expand" switch.
 */

#include "slidehpanel.h"

SlideHPanel::SlideHPanel(QWidget *parent)
    : SlidePanel(parent)
{
    position = PANEL_TOP;
    invisibleMargin = 12;
    panelHeight = 100;
    slideAmount = 40;
    mLayout.setContentsMargins(0,0,0,0);
    recalculateGeometry();
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
        saveStaticGeometry(QRect(QPoint(0, 0),
                                 QPoint(containerSize().width() - 1,
                                        panelHeight - 1 + invisibleMargin)));
        this->setGeometry(staticGeometry());
        initialPosition = geometry().topLeft();
        slideAnimation->setStartValue(initialPosition);
        slideAnimation->setEndValue(initialPosition - QPoint(0, slideAmount));
    } else {
        saveStaticGeometry(QRect(QPoint(0, containerSize().height() - panelHeight + 1),
                                QPoint(containerSize().width(), containerSize().height()) - QPoint(0,1) ));
        this->setGeometry(staticGeometry());
        initialPosition = geometry().topLeft(); //QPoint(0, containerSize().height() - height());
        slideAnimation->setStartValue(initialPosition);
        slideAnimation->setEndValue(QPoint(0, containerSize().height() - height() + slideAmount));
    }
    updateTriggerRect();
}

void SlideHPanel::updateTriggerRect() {
    if(position == PANEL_TOP) {
        mTriggerRect = staticGeometry();
    }
    else {
        // adjust so it wont interfere with MapOverlay in bottom right corner
        mTriggerRect = staticGeometry().adjusted(0, 0, -180, 0);
    }
}
