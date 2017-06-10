#include "slidevpanel.h"

SlideVPanel::SlideVPanel(QWidget *w)
    : SlidePanel(w)
{
    position = RIGHT;
    mLayout.setContentsMargins(0,0,0,0);
    recalculateGeometry();
}

SlideVPanel::~SlideVPanel() {
}

void SlideVPanel::containerResized(QSize parentSz) {
    this->parentSz = parentSz;
    recalculateGeometry();
    // TODO: expand flag. leaving like this for now
    //mWidget->resize(this->width(), mWidget->height());
}

QSize SlideVPanel::triggerSize() {
    return this->size();
}

void SlideVPanel::setPosition(PanelVPosition p) {
    position = p;
    recalculateGeometry();
}

void SlideVPanel::recalculateGeometry() {
    if(position == RIGHT) {
        setGeometry(parentSz.width() - width(), parentSz.height()/2 - height()/2, width(), height());
        initialPosition = geometry().topLeft();
        slideAnimation->setStartValue(initialPosition);
        slideAnimation->setEndValue(QPoint(initialPosition.x() + 20, initialPosition.y()));
    } else {
        setGeometry(0, parentSz.height()/2 - height()/2, width(), height());
        initialPosition = geometry().topLeft();
        slideAnimation->setStartValue(initialPosition);
        slideAnimation->setEndValue(QPoint(initialPosition.x() - 20, initialPosition.y()));
    }
}

void SlideVPanel::paintEvent(QPaintEvent *event) {
    QWidget::paintEvent(event);
    if(position == PanelHPosition::TOP) {
        QPainter p(this);
        p.setPen(QColor(QColor(40, 255, 40)));
        p.drawRect(rect());
    }
}
