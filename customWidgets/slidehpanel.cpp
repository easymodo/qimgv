/*
 * This panel will use sizeHint().height() from the widget it contains.
 * Width will match parent's assuming you called containerResized(QSize).
 * TODO: implement "expand" switch.
 */

#include "slidehpanel.h"

SlideHPanel::SlideHPanel(QWidget *parent)
    : SlidePanel(parent)
{
    position = TOP;
    invisibleMargin = 12;
    panelHeight = 100;
    layout.setContentsMargins(0,0,0,0);
    recalculateGeometry();
}

SlideHPanel::~SlideHPanel() {

}

void SlideHPanel::containerResized(QSize parentSz) {
    this->parentSz = parentSz;
    recalculateGeometry();
    // expand widget to full width
    //if(mWidget)
    //    mWidget->resize(parentSz.width(), mWidget->height());
}

QSize SlideHPanel::triggerSize() {
    if(position == TOP)
        return QSize(width(), height() - invisibleMargin);
    else
        return size();
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
    qDebug() << "recalc: " << panelHeight;
    if(position == TOP) {
        this->setGeometry(QRect(QPoint(0, 0),
                                QPoint(parentSz.width(), panelHeight - 1 + invisibleMargin)));
        initialPosition = geometry().topLeft();
        slideAnimation->setStartValue(initialPosition);
        slideAnimation->setEndValue(QPoint(0, - 30));
    } else {
        this->setGeometry(QRect(QPoint(0, parentSz.height() - panelHeight + 1),
                                QPoint(parentSz.width(), parentSz.height())));
        initialPosition = QPoint(0, parentSz.height() - height());
        slideAnimation->setStartValue(initialPosition);
        slideAnimation->setEndValue(QPoint(0, parentSz.height() - height() + 30));
    }
}
