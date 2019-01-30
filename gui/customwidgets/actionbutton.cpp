#include "actionbutton.h"

ActionButton::ActionButton(QWidget *parent)
    : QPushButton(parent),
      mTriggerMode(TriggerMode::ClickTrigger)
{
    this->setContentsMargins(0,0,0,0);
    this->setFocusPolicy(Qt::NoFocus);
}

ActionButton::ActionButton(QString _actionName, QString _iconPath, QWidget *parent)
    :  ActionButton(parent)
{
    this->setIcon(QIcon(_iconPath));
    setAction(_actionName);
}

ActionButton::ActionButton(QString _actionName, QString _iconPath, int _size, QWidget *parent)
    :  ActionButton(_actionName, _iconPath, parent)
{
    if(_size > 0)
        setFixedSize(_size, _size);
}

void ActionButton::setAction(QString _actionName) {
    actionName = _actionName;
}

void ActionButton::setIcon(QIcon icon) {
    QPushButton::setIcon(icon);
    setIconSize(icon.availableSizes().first());
}

void ActionButton::setTriggerMode(TriggerMode mode) {
    mTriggerMode = mode;
}

TriggerMode ActionButton::triggerMode() {
    return mTriggerMode;
}

void ActionButton::mousePressEvent(QMouseEvent *event) {
    QPushButton::mousePressEvent(event);
    if(mTriggerMode == TriggerMode::PressTrigger && event->button() == Qt::LeftButton)
        actionManager->invokeAction(actionName);
}

void ActionButton::mouseReleaseEvent(QMouseEvent *event) {
    QPushButton::mouseReleaseEvent(event);
    if(mTriggerMode == TriggerMode::ClickTrigger && rect().contains(event->pos()) && event->button() == Qt::LeftButton)
        actionManager->invokeAction(actionName);
}
