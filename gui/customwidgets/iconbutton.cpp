#include "iconbutton.h"

IconButton::IconButton(QWidget *parent)
    :  QLabel(parent)
{
    this->setContentsMargins(0,0,0,0);
    this->setAlignment(Qt::AlignCenter);
    this->setAccessibleName("iconButton");
}

IconButton::IconButton(QString _actionName, QString _iconPath, QWidget *parent)
    :  IconButton(parent)
{
    this->setPixmap(QPixmap(_iconPath));
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

void IconButton::mousePressEvent(QMouseEvent *event) {
    Q_UNUSED (event)
    if(event->button() == Qt::LeftButton)
        actionManager->invokeAction(actionName);
}

/*
QSize IconButton::sizeHint() const {
    QSize s = size();
    lastHeight = s.height();
    //s.setWidth((s.height()*16)/9);
    s.setWidth(s.height());
    s.setHeight(QLabel::sizeHint().height());
    return s;
}

void IconButton::resizeEvent(QResizeEvent * event) {
    QLabel::resizeEvent(event);

    if(lastHeight!=height()) {
        updateGeometry(); // it is possible that this call should be scheduled to next iteration of event loop
    }
}
*/
