/*
 * This panel will use sizeHint().height() from the widget it contains.
 * Width will match containerSize()Size.width()
 * TODO: implement "expand" switch.
 */

#include "slidehpanel.h"

SlideHPanel::SlideHPanel(QWidget *parent)
    : SlidePanel(parent)
{
    position = TOP;
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
    if(position == TOP) {
        this->setGeometry(QRect(QPoint(0, 0),
                                QPoint(containerSize().width() - 1, panelHeight - 1 + invisibleMargin)));
        initialPosition = geometry().topLeft();
        slideAnimation->setStartValue(initialPosition);
        slideAnimation->setEndValue(initialPosition - QPoint(0, slideAmount));
    } else {
        this->setGeometry(QRect(QPoint(0, containerSize().height() - panelHeight + 1),
                                QPoint(containerSize().width(), containerSize().height()) - QPoint(0,1) ));
        initialPosition = geometry().topLeft(); //QPoint(0, containerSize().height() - height());
        slideAnimation->setStartValue(initialPosition);
        slideAnimation->setEndValue(QPoint(0, containerSize().height() - height() + slideAmount));
    }
    updateTriggerRect();
}

void SlideHPanel::updateTriggerRect() {
    if(position == TOP) {
        mTriggerRect = geometry().adjusted(0, 0, 0, 0);
    }
    else {
        // adjust so it wont interfere with MapOverlay in bottom right corner
        mTriggerRect = geometry().adjusted(0, 0, -180, 0);
    }
}
