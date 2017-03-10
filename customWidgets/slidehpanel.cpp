#include "slidehpanel.h"

SlideHPanel::SlideHPanel(QWidget *w, QWidget *parent)
    : SlidePanel(w, parent)
{
    position = TOP;
}

SlideHPanel::~SlideHPanel() {

}

void SlideHPanel::updatePanelPosition() {
    QRect oldRect = this->rect();
    if(position == TOP) {
        this->setGeometry(QRect(QPoint(0, 0),
                                QPoint(parentSz.width(), mWidget->height() - 1)));
        initialPosition = geometry().topLeft();
        slideAnimation->setStartValue(initialPosition);
        slideAnimation->setEndValue(QPoint(0, -60));
    } else {
        this->setGeometry(QRect(QPoint(0, parentSz.height() - mWidget->height() + 1),
                                QPoint(parentSz.width(), parentSz.height())));
        initialPosition = QPoint(0, parentSz.height() - height());
        slideAnimation->setStartValue(initialPosition);
        slideAnimation->setEndValue(QPoint(0, parentSz.height() - height() + 60));
    }

    if(oldRect != this->rect())
        emit panelSizeChanged();
}

void SlideHPanel::setPosition(PanelHPosition p) {
    position = p;
    updatePanelPosition();
}

void SlideHPanel::parentResized(QSize parentSz) {
    this->parentSz = parentSz;
    updatePanelPosition();
    // expand
    mWidget->resize(parentSz.width(), mWidget->height());
}
