#include "actionbutton.h"

ActionButton::ActionButton(QWidget *parent)
    : ClickableLabel(parent),
      mTriggerMode(TriggerMode::ClickTrigger),
      checkable(false)
{
    this->setContentsMargins(0,0,0,0);
    this->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    this->setFocusPolicy(Qt::NoFocus);
    this->setProperty("checked", false);
}

ActionButton::ActionButton(QString _actionName, QString _iconPath, QWidget *parent)
    :  ActionButton(parent)
{
    setPixmap(QPixmap(_iconPath));
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

/*
void ActionButton::setPixmap(QPixmap icon) {
    //setIconSize(icon.availableSizes().first());
}
*/

void ActionButton::setTriggerMode(TriggerMode mode) {
    mTriggerMode = mode;
}

TriggerMode ActionButton::triggerMode() {
    return mTriggerMode;
}

void ActionButton::setCheckable(bool mode) {
    checkable = mode;
}

void ActionButton::mousePressEvent(QMouseEvent *event) {
    ClickableLabel::mousePressEvent(event);
    if(mTriggerMode == TriggerMode::PressTrigger && event->button() == Qt::LeftButton)
        actionManager->invokeAction(actionName);
}

void ActionButton::mouseReleaseEvent(QMouseEvent *event) {
    ClickableLabel::mouseReleaseEvent(event);
    if(mTriggerMode == TriggerMode::ClickTrigger && rect().contains(event->pos()) && event->button() == Qt::LeftButton)
        actionManager->invokeAction(actionName);
    if(checkable) {
        bool mode = !this->property("checked").toBool();
        setProperty("checked", mode);
        style()->unpolish(this);
        style()->polish(this);
        emit toggled(mode);
    }
}
