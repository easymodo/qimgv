#include "iconbutton.h"

IconButton::IconButton(QWidget *parent)
    : IconWidget(parent),
      checkable(false)
{
}

void IconButton::setCheckable(bool mode) {
    checkable = mode;
}

void IconButton::mousePressEvent(QMouseEvent *event) {
    Q_UNUSED(event)
    emit pressed();
}

void IconButton::mouseReleaseEvent(QMouseEvent *event) {
    Q_UNUSED(event)
    if(checkable) {
        bool mode = !this->property("checked").toBool();
        setProperty("checked", mode);
        style()->unpolish(this);
        style()->polish(this);
        emit toggled(mode);
    } else {
        emit clicked();
    }
}
