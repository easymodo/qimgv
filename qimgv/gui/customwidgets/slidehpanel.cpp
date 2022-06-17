#include "slidehpanel.h"

SlideHPanel::SlideHPanel(FloatingWidgetContainer *parent) : SlidePanel(parent) {
    mLayout.setContentsMargins(0,0,0,0);
    setPosition(PANEL_TOP);
}

SlideHPanel::~SlideHPanel() {

}

QRect SlideHPanel::triggerRect() {
    return mTriggerRect;
}

void SlideHPanel::setPosition(PanelHPosition p) {
    mPosition = p;
    recalculateGeometry();
}

PanelHPosition SlideHPanel::position() {
    return mPosition;
}

void SlideHPanel::recalculateGeometry() {
    if(layoutManaged())
        return;
    if(mPosition == PANEL_TOP) {
        setAnimationRange(QPoint(0,0),
                          QPoint(0,0) - QPoint(0, slideAmount));
        saveStaticGeometry(QRect(QPoint(0, 0),
                                 QPoint(containerSize().width() - 1, height() - 1)));
    } else {
        setAnimationRange(QPoint(0, containerSize().height() - height()),
                          QPoint(0, containerSize().height() - height() + slideAmount));
        saveStaticGeometry(QRect(QPoint(0, containerSize().height() - height()),
                                 QPoint(containerSize().width() - 1, containerSize().height())));
    }
    this->setGeometry(staticGeometry());
    updateTriggerRect();
}

void SlideHPanel::updateTriggerRect() {
    mTriggerRect = staticGeometry();
}
