#include "overlaymessage.h"

OverlayMessage::OverlayMessage(QWidget *parent) : InfoOverlay(parent) {
    position = Message::POSITION_LEFT;
    icon = Message::ICON_LEFT_EDGE;
    currentIcon = NULL;
    currentOpacity = 1.0f;
    duration = 1200;
    textMarginX = 10;
    textMarginY = 6;
    bgColor.setRgb(20,20,20, 255);
    borderColor.setRgb(230,230,230, 255);
    //borderColor.setRgb(30,30,30, 255);
    textColor.setRgb(235, 235, 235, 255);
    setFontSize(17);
    ///////////////// icons
    iconLeftEdge.load(":/res/icons/message/left_edge32.png");
    iconRightEdge.load(":/res/icons/message/right_edge32.png");
    setIcon(Message::NO_ICON);
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

void OverlayMessage::showMessage(QString _text, Message::Position _position, Message::Icon _icon, int _duration) {
    position = _position;
    duration = _duration;
    setIcon(_icon);
    setText(_text);
    show();
}

void OverlayMessage::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event)
    QPainter painter(this);
    //painter.setRenderHint(QPainter::Antialiasing);
    painter.setOpacity(currentOpacity);
    QPainterPath path;
    path.addRoundedRect(rect(), 5, 5);
    painter.fillPath(path, bgColor);
    //painter.setCompositionMode(QPainter::CompositionMode_Source);

    if(icon != Message::NO_ICON && currentIcon)
        painter.drawPixmap(iconRect, *currentIcon);

    if(!text.isEmpty()) {
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
    int padding = 6;
    QRect newRect;
    if(icon == Message::NO_ICON) {
        newRect = QRect(QPoint(0, 0), QSize(fm->width(text) + textMarginX * 2, fm->height() + textMarginY * 2));
        textRect = newRect.adjusted(textMarginX, textMarginY, -textMarginX, -textMarginY);
    } else {
        // we assume that fm->height() < iconSize
        iconRect = QRect(QPoint(padding, padding), QSize(iconSize, iconSize));
        // text will be on the right side
        textRect = QRect(0,0, fm->width(text) + textMarginX * 2, fm->height());
        if(text.isEmpty())
            newRect = QRect(QPoint(0, 0), QSize(iconSize + padding * 2, iconSize + padding * 2));
        else
            newRect = QRect(QPoint(0, 0), QSize(iconSize + padding * 2 + textRect.width(), iconSize + padding * 2));
        textRect.moveCenter(newRect.center() + QPoint(iconSize + textMarginX + padding, 0) / 2);
    }

    QPoint pos(0, 0);
    switch (position) {
    case Message::POSITION_LEFT:
        pos.setX(marginX);
        pos.setY( (containerSize().height() - newRect.height()) / 2);
        break;
    case Message::POSITION_RIGHT:
        pos.setX(containerSize().width() - newRect.width() - marginX);
        pos.setY( (containerSize().height() - newRect.height()) / 2);
        break;
    case Message::POSITION_BOTTOM:
        pos.setX( (containerSize().width() - newRect.width()) / 2);
        pos.setY(containerSize().height() - newRect.height() - marginY);
        break;
    case Message::POSITION_TOP:
        pos.setX( (containerSize().width() - newRect.width()) / 2);
        pos.setY(marginY);
        break;
    case Message::POSITION_TOPLEFT:
        pos.setX(marginX);
        pos.setY(marginY);
        break;
    case Message::POSITION_TOPRIGHT:
        pos.setX(containerSize().width() - newRect.width() - marginX);
        pos.setY(marginY);
        break;
    case Message::POSITION_BOTTOMLEFT:
        pos.setX(marginX);
        pos.setY(containerSize().height() - newRect.height() - marginY);
        break;
    case Message::POSITION_BOTTOMRIGHT:
        pos.setX(containerSize().width() - newRect.width() - marginX);
        pos.setY(containerSize().height() - newRect.height() - marginY);
        break;
    }
    // apply position
    newRect.moveTopLeft(pos);
    setGeometry(newRect);
}

void OverlayMessage::setIcon(Message::Icon _icon) {
    icon = _icon;
    switch (icon) {
        case Message::NO_ICON: break;
        case Message::ICON_LEFT_EDGE: currentIcon = &iconLeftEdge; break;
        case Message::ICON_RIGHT_EDGE: currentIcon = &iconRightEdge; break;
    }
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
