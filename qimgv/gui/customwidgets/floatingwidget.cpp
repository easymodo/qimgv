#include "floatingwidget.h"

FloatingWidget::FloatingWidget(OverlayContainerWidget *parent)
    : OverlayWidget(parent),
      marginX(20),
      marginY(35),
      fadeEnabled(false)
{
    opacityEffect = new QGraphicsOpacityEffect(this);
    opacityEffect->setOpacity(1.0);
    this->setGraphicsEffect(opacityEffect);
    fadeAnimation = new QPropertyAnimation(this, "opacity");
    fadeAnimation->setDuration(120);
    fadeAnimation->setStartValue(1.0f);
    fadeAnimation->setEndValue(0.0f);
    fadeAnimation->setEasingCurve(QEasingCurve::OutQuad);
    connect(fadeAnimation, SIGNAL(finished()), this, SLOT(onFadeEnd()), Qt::UniqueConnection);
}

FloatingWidget::~FloatingWidget() {
    delete opacityEffect;
    delete fadeAnimation;
}

qreal FloatingWidget::opacity() const {
    return opacityEffect->opacity();
}

void FloatingWidget::setOpacity(qreal opacity) {
    opacityEffect->setOpacity(opacity);
    update();
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

void FloatingWidget::setFadeDuration(int duration) {
    fadeAnimation->setDuration(duration);
}

void FloatingWidget::setFadeEnabled(bool mode) {
    this->fadeEnabled = mode;
}

void FloatingWidget::show() {
    fadeAnimation->stop();
    opacityEffect->setOpacity(1.0);
    QWidget::show();
}

void FloatingWidget::hide() {
    if(fadeEnabled && !this->isHidden()) {
        fadeAnimation->stop();
        fadeAnimation->start(QPropertyAnimation::KeepWhenStopped);
    } else {
        QWidget::hide();
    }
}

void FloatingWidget::onFadeEnd() {
    QWidget::hide();
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
