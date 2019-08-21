#include "slidevpanel.h"

SlideVPanel::SlideVPanel(OverlayContainerWidget *w)
    : SlidePanel(w)
{
    mLayout.setContentsMargins(0,0,0,0);
    setPosition(PANEL_RIGHT);
}

SlideVPanel::~SlideVPanel() {
}

QRect SlideVPanel::triggerRect() {
    return mTriggerRect;
}

void SlideVPanel::setPosition(PanelVPosition p) {
    position = p;
    if(position == PANEL_RIGHT) {
        QPoint start = QPoint(0,0);
        setAnimationRange(start,
                          start + QPoint(20,0));
    } else {
        QPoint start = QPoint(0,0);
        setAnimationRange(start,
                          start + QPoint(-20,0));
    }
    recalculateGeometry();
}

// TODO: this may be incorrect.
void SlideVPanel::recalculateGeometry() {
    if(position == PANEL_RIGHT)
        saveStaticGeometry(QRect(containerSize().width() - width(), containerSize().height()/2 - height()/2, width(), height()));
    else
        saveStaticGeometry(QRect(0, containerSize().height()/2 - height()/2, width(), height()));
    setGeometry(staticGeometry());
    updateTriggerRect();
}

void SlideVPanel::updateTriggerRect() {
    mTriggerRect = staticGeometry();
}
