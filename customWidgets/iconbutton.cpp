#include "iconbutton.h"

IconButton::IconButton() {
    this->setAccessibleName("iconButton");
}

IconButton::IconButton(QString _actionName, QString _iconPath) {
    IconButton();
    this->setPixmap(QPixmap(_iconPath));
}

IconButton::IconButton(QString _actionName, QString _iconPath, QWidget *parent)
    :  QLabel(parent)
{
    IconButton(_actionName, _iconPath);
}

void IconButton::setAction(QString _actionName) {
    actionName = _actionName;
}

void IconButton::mousePressEvent(QMouseEvent *event) {
    Q_UNUSED (event)
    if(event->button() == Qt::LeftButton)
        actionManager->invokeAction(actionName);
}
