#include "iconbutton.h"

IconButton::IconButton(QWidget *parent)
    : QPushButton(parent)
{
    this->setContentsMargins(0,0,0,0);
    this->setAccessibleName("IconButton");
    this->setFocusPolicy(Qt::NoFocus);
}

IconButton::IconButton(QString _actionName, QString _iconPath, QWidget *parent)
    :  IconButton(parent)
{
    this->setIcon(QIcon(_iconPath));
    setAction(_actionName);
}

IconButton::IconButton(QString _actionName, QString _iconPath, int _size, QWidget *parent)
    :  IconButton(_actionName, _iconPath, parent)
{
    if(_size > 0)
        setFixedSize(_size, _size);
}

void IconButton::setAction(QString _actionName) {
    actionName = _actionName;
}

void IconButton::setIcon(QIcon icon) {
    QPushButton::setIcon(icon);
    setIconSize(icon.availableSizes().first());
}

void IconButton::mousePressEvent(QMouseEvent *event) {
    QPushButton::mousePressEvent(event);
    if(event->button() == Qt::LeftButton)
        actionManager->invokeAction(actionName);
}
