#include "overlaymessage.h"

OverlayMessage::OverlayMessage(QWidget *parent) : InfoOverlay(parent) {
    currentOpacity = 1.0f;
    duration = 1200;
    position = MESSAGE_LEFT;
    textMarginX = 10;
    textMarginY = 6;
    bgColor.setRgb(20,20,20, 255);
    borderColor.setRgb(30,30,30, 255);
    textColor.setRgb(235, 235, 235, 255);
    setFontSize(17);
////////////////////////////////////////////////////////////////////////////////
    //effectColor.setRgb(255, 124, 0);
    //effectColor.setRgb(227, 132, 0);
    effectColor.setRgb(110,110,110);
    effectStrength = 0.0f;
    colorizeEffect = new QGraphicsColorizeEffect(this);
    colorizeEffect->setColor(effectColor);
    colorizeEffect->setStrength(effectStrength);
    setGraphicsEffect(colorizeEffect);

    colorizeAnimation = new QPropertyAnimation(colorizeEffect, "strength");
    colorizeAnimation->setEasingCurve(QEasingCurve::OutQuad);
    colorizeAnimation->setDuration(duration);
    colorizeAnimation->setStartValue(1.0f);
    colorizeAnimation->setEndValue(0.0f);

    fadeAnimation = new QPropertyAnimation(this, "opacity");
    fadeAnimation->setDuration(duration);
    fadeAnimation->setStartValue(1.0f);
    fadeAnimation->setEndValue(0.0f);
    fadeAnimation->setEasingCurve(QEasingCurve::InQuad);

    animGroup = new QParallelAnimationGroup;
    animGroup->addAnimation(colorizeAnimation);
    animGroup->addAnimation(fadeAnimation);

    connect(animGroup, SIGNAL(finished()), this, SLOT(hide()), Qt::UniqueConnection);
////////////////////////////////////////////////////////////////////////////////
}

void OverlayMessage::showMessage(QString _text, MessagePosition _position, int _duration) {
    position = _position;
    duration = _duration;
    setText(_text);
    show();
}

void OverlayMessage::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event)
    if(!text.isEmpty()) {
        QPainter painter(this);
        painter.setOpacity(currentOpacity);
        QRect rect = QRect(0,0, width(), height());
        painter.fillRect(rect.adjusted(2,2,-2,-2), QBrush(bgColor));
        painter.setPen(QPen(borderColor));
        painter.drawRect(rect.adjusted(0,0,-1,-1));
        painter.drawRect(rect.adjusted(1,1,-2,-2));
        painter.setFont(font);
        painter.setPen(QPen(textShadowColor));
        painter.drawText(textRect.adjusted(1,1,1,1), Qt::TextSingleLine, text);
        painter.setPen(QPen(textColor));
        painter.drawText(textRect, Qt::TextSingleLine, text);
    }
}

float OverlayMessage::opacity() const {
    return currentOpacity;
}

void OverlayMessage::setOpacity(float _opacity) {
    currentOpacity = _opacity;
    update();
}

void OverlayMessage::recalculateGeometry() {
    QRect newRect = QRect(QPoint(0, 0), QSize(fm->width(text) + textMarginX * 2, fm->height() + textMarginY * 2));
    textRect = newRect.adjusted(textMarginX, textMarginY, -textMarginX, -textMarginY);

    QPoint pos(0, 0);
    switch (position) {
    case MESSAGE_LEFT:
        pos.setX(marginX);
        pos.setY( (containerSize().height() - newRect.height()) / 2);
        break;
    case MESSAGE_RIGHT:
        pos.setX(containerSize().width() - newRect.width() - marginX);
        pos.setY( (containerSize().height() - newRect.height()) / 2);
        break;
    case MESSAGE_BOTTOM:
        pos.setX( (containerSize().width() - newRect.width()) / 2);
        pos.setY(containerSize().height() - newRect.height() - marginY);
        break;
    case MESSAGE_TOP:
        pos.setX( (containerSize().width() - newRect.width()) / 2);
        pos.setY(marginY);
        break;
    case MESSAGE_TOPLEFT:
        pos.setX(marginX);
        pos.setY(marginY);
        break;
    case MESSAGE_TOPRIGHT:
        pos.setX(containerSize().width() - newRect.width() - marginX);
        pos.setY(marginY);
        break;
    case MESSAGE_BOTTOMLEFT:
        pos.setX(marginX);
        pos.setY(containerSize().height() - newRect.height() - marginY);
        break;
    case MESSAGE_BOTTOMRIGHT:
        pos.setX(containerSize().width() - newRect.width() - marginX);
        pos.setY(containerSize().height() - newRect.height() - marginY);
        break;
    }
    // apply position
    newRect.moveTopLeft(pos);
    setGeometry(newRect);
}

void OverlayMessage::mousePressEvent(QMouseEvent *event) {
    Q_UNUSED (event)
}

void OverlayMessage::show() {
    animGroup->stop();
    QWidget::show();
    colorizeAnimation->setDuration(duration);
    fadeAnimation->setDuration(duration);
    animGroup->start(QPropertyAnimation::KeepWhenStopped);
}
