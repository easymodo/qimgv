#include "slidevpanel.h"

SlideVPanel::SlideVPanel(QWidget *w, QWidget *parent)
    : SlidePanel(w, parent)
{
    position = RIGHT;
}

SlideVPanel::~SlideVPanel() {

}

void SlideVPanel::updatePanelPosition() {
    QRect oldRect = this->rect();
    if(position == RIGHT) { // TODO: expand enum to use left/right
        // RIGHT
        setGeometry(parentSz.width() - width(), parentSz.height()/2 - height()/2, width(), height());
        initialPosition = geometry().topLeft();
        slideAnimation->setStartValue(initialPosition);
        slideAnimation->setEndValue(QPoint(initialPosition.x() + 20, initialPosition.y()));
        qDebug() << this->geometry();
    } else {
        // LEFT
        setGeometry(0, parentSz.height()/2 - height()/2, width(), height());
        initialPosition = geometry().topLeft();
        slideAnimation->setStartValue(initialPosition);
        slideAnimation->setEndValue(QPoint(initialPosition.x() - 20, initialPosition.y()));
        qDebug() << this->geometry();
    }

    if(oldRect != this->rect())
        emit panelSizeChanged();
}

void SlideVPanel::setPosition(PanelVPosition p) {
    position = p;
    updatePanelPosition();
}

void SlideVPanel::parentResized(QSize parentSz) {
    this->parentSz = parentSz;
    updatePanelPosition();
    // TODO: expand flag. leaving like this for now
    //mWidget->resize(this->width(), mWidget->height());
}

void SlideVPanel::paintEvent(QPaintEvent *event) {
    QWidget::paintEvent(event);
    if(position == PanelHPosition::TOP) {
        QPainter p(this);
        p.setPen(QColor(QColor(40, 255, 40)));
        p.drawRect(rect());
    }
}
