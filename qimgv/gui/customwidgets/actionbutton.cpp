#include "actionbutton.h"

ActionButton::ActionButton(QWidget *parent)
    : IconButton(parent),
      mTriggerMode(TriggerMode::ClickTrigger)
{
    this->setFocusPolicy(Qt::NoFocus);
    this->setProperty("checked", false);
}

ActionButton::ActionButton(QString _actionName, QString _iconPath, QWidget *parent)
    :  ActionButton(parent)
{
    setIconPath(_iconPath);
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

void ActionButton::setTriggerMode(TriggerMode mode) {
    mTriggerMode = mode;
}

TriggerMode ActionButton::triggerMode() {
    return mTriggerMode;
}

void ActionButton::mousePressEvent(QMouseEvent *event) {
    IconButton::mousePressEvent(event);
    if(mTriggerMode == TriggerMode::PressTrigger && event->button() == Qt::LeftButton)
        actionManager->invokeAction(actionName);
}

void ActionButton::mouseReleaseEvent(QMouseEvent *event) {
    IconButton::mouseReleaseEvent(event);
    if(mTriggerMode == TriggerMode::ClickTrigger && rect().contains(event->pos()) && event->button() == Qt::LeftButton)
        actionManager->invokeAction(actionName);

}
