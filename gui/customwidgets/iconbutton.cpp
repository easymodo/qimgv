#include "iconbutton.h"

IconButton::IconButton() {
    this->setFixedSize(30, 30);
    this->setContentsMargins(0,0,0,0);
    this->setAccessibleName("iconButton");
}

IconButton::IconButton(QString _actionName, QString _iconPath) {
    this->setFixedSize(30, 30);
    this->setContentsMargins(0,0,0,0);
    this->setAccessibleName("iconButton");
    this->setPixmap(QPixmap(_iconPath));
    setAction(_actionName);
}

IconButton::IconButton(QString _actionName, QString _iconPath, QWidget *parent)
    :  QLabel(parent)
{
    this->setFixedSize(30, 30);
    this->setContentsMargins(0,0,0,0);
    this->setAccessibleName("iconButton");
    this->setPixmap(QPixmap(_iconPath));
    setAction(_actionName);
}

void IconButton::setAction(QString _actionName) {
    actionName = _actionName;
}

void IconButton::mousePressEvent(QMouseEvent *event) {
    Q_UNUSED (event)
    if(event->button() == Qt::LeftButton)
        actionManager->invokeAction(actionName);
}
