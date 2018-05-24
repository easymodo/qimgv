#include "floatingwidget.h"

FloatingWidget::FloatingWidget(OverlayContainerWidget *parent)
    : OverlayWidget(parent),
      marginX(20),
      marginY(30)
{
}

void FloatingWidget::setMarginX(int margin) {
    marginX = margin;
    recalculateGeometry();
}

void FloatingWidget::setMarginY(int margin) {
    marginY = margin;
    recalculateGeometry();
}

void FloatingWidget::setPosition(FloatingWidgetPosition pos) {
    position = pos;
    recalculateGeometry();
}

void FloatingWidget::recalculateGeometry() {
    QRect newRect = QRect(QPoint(0,0), sizeHint());
    QPoint pos(0, 0);
    switch (position) {
        case FloatingWidgetPosition::LEFT:
            pos.setX(marginX);
            pos.setY( (containerSize().height() - newRect.height()) / 2);
            break;
        case FloatingWidgetPosition::RIGHT:
            pos.setX(containerSize().width() - newRect.width() - marginX);
            pos.setY( (containerSize().height() - newRect.height()) / 2);
            break;
        case FloatingWidgetPosition::BOTTOM:
            pos.setX( (containerSize().width() - newRect.width()) / 2);
            pos.setY(containerSize().height() - newRect.height() - marginY);
            break;
        case FloatingWidgetPosition::TOP:
            pos.setX( (containerSize().width() - newRect.width()) / 2);
            pos.setY(marginY);
            break;
        case FloatingWidgetPosition::TOPLEFT:
            pos.setX(marginX);
            pos.setY(marginY);
            break;
        case FloatingWidgetPosition::TOPRIGHT:
            pos.setX(containerSize().width() - newRect.width() - marginX);
            pos.setY(marginY);
            break;
        case FloatingWidgetPosition::BOTTOMLEFT:
            pos.setX(marginX);
            pos.setY(containerSize().height() - newRect.height() - marginY);
            break;
        case FloatingWidgetPosition::BOTTOMRIGHT:
            pos.setX(containerSize().width() - newRect.width() - marginX);
            pos.setY(containerSize().height() - newRect.height() - marginY);
            break;
        case FloatingWidgetPosition::CENTER:
            pos.setX( (containerSize().width() - newRect.width()) / 2);
            pos.setY( (containerSize().height() - newRect.height()) / 2);
    }
    // apply position
    newRect.moveTopLeft(pos);
    setGeometry(newRect);
}
