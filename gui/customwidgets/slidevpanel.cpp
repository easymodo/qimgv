#include "slidevpanel.h"

SlideVPanel::SlideVPanel(QWidget *w)
    : SlidePanel(w)
{
    slideAmount = 20;
    position = PANEL_RIGHT;
    mLayout.setContentsMargins(0,0,0,0);
    recalculateGeometry();
}

SlideVPanel::~SlideVPanel() {
}

QRect SlideVPanel::triggerRect() {
    return mTriggerRect;
}

void SlideVPanel::setPosition(PanelVPosition p) {
    position = p;
    recalculateGeometry();
}

// TODO: this may be incorrect.
void SlideVPanel::recalculateGeometry() {
    if(position == PANEL_RIGHT) {
        saveStaticGeometry(QRect(containerSize().width() - width(), containerSize().height()/2 - height()/2, width(), height()));
        setGeometry(staticGeometry());
        initialPosition = geometry().topLeft();
        slideAnimation->setStartValue(initialPosition);
        slideAnimation->setEndValue(QPoint(initialPosition.x() + 20, initialPosition.y()));
    } else {
        saveStaticGeometry(QRect(0, containerSize().height()/2 - height()/2, width(), height()));
        setGeometry(staticGeometry());
        initialPosition = geometry().topLeft();
        slideAnimation->setStartValue(initialPosition);
        slideAnimation->setEndValue(QPoint(initialPosition.x() - 20, initialPosition.y()));
    }
    updateTriggerRect();
}

void SlideVPanel::updateTriggerRect() {
    mTriggerRect = staticGeometry();
}

void SlideVPanel::paintEvent(QPaintEvent *event) {
    QWidget::paintEvent(event);
    /*
    if(position == PanelVPosition::LEFT) {
        QPainter p(this);
        p.setPen(QColor(QColor(40, 255, 40)));
        p.drawRect(rect());
    }
    */
}
