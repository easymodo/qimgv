#include "floatingmessage.h"

FloatingMessage::FloatingMessage(QWidget *parent) : InfoOverlay(parent) {
    position = Message::POSITION_LEFT;
    icon = Message::ICON_LEFT_EDGE;
    currentIcon = NULL;
    currentOpacity = 1.0f;
    duration = 1200;
    textMarginX = 10;
    textMarginY = 6;
    bgColor.setRgb(55,55,55, 255);
    borderColor.setRgb(100,100,100, 255);
    textColor.setRgb(235, 235, 235, 255);
    setFontSize(17);
    ///////////////// icons
    iconLeftEdge.load(":/res/icons/message/left_edge32.png");
    iconRightEdge.load(":/res/icons/message/right_edge32.png");
    setIcon(Message::NO_ICON);

    fadeAnimation = new QPropertyAnimation(this, "opacity");
    fadeAnimation->setDuration(duration);
    fadeAnimation->setStartValue(1.0f);
    fadeAnimation->setEndValue(0.0f);
    fadeAnimation->setEasingCurve(QEasingCurve::InQuad);

    animGroup = new QParallelAnimationGroup;
    animGroup->addAnimation(fadeAnimation);

    connect(animGroup, SIGNAL(finished()), this, SLOT(hide()), Qt::UniqueConnection);
}

void FloatingMessage::showMessage(QString _text, Message::Position _position, Message::Icon _icon, int _duration) {
    position = _position;
    duration = _duration;
    setIcon(_icon);
    setText(_text);
    show();
}

void FloatingMessage::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event)
    QPainter painter(this);
    painter.setOpacity(currentOpacity);
    QPainterPath path, path2;
    path.addRoundedRect(rect(), 3, 3);
    path2.addRoundedRect(rect().adjusted(1,1,-1,-1), 3, 3);
    painter.fillPath(path, borderColor);
    painter.fillPath(path2, bgColor);

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

float FloatingMessage::opacity() const {
    return currentOpacity;
}

void FloatingMessage::setOpacity(float _opacity) {
    currentOpacity = _opacity;
    update();
}

void FloatingMessage::recalculateGeometry() {
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

void FloatingMessage::setIcon(Message::Icon _icon) {
    icon = _icon;
    switch (icon) {
        case Message::NO_ICON: break;
        case Message::ICON_LEFT_EDGE: currentIcon = &iconLeftEdge; break;
        case Message::ICON_RIGHT_EDGE: currentIcon = &iconRightEdge; break;
    }
}

void FloatingMessage::mousePressEvent(QMouseEvent *event) {
    Q_UNUSED (event)
}

void FloatingMessage::show() {
    animGroup->stop();
    QWidget::show();
    fadeAnimation->setDuration(duration);
    animGroup->start(QPropertyAnimation::KeepWhenStopped);
}
