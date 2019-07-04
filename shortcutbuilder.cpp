#include "shortcutbuilder.h"

QString ShortcutBuilder::fromEvent(QEvent *event) {
    QKeyEvent *keyEvent = dynamic_cast<QKeyEvent *>(event);
    QWheelEvent *wheelEvent = dynamic_cast<QWheelEvent *>(event);
    QMouseEvent *mouseEvent = dynamic_cast<QMouseEvent *>(event);
    if(keyEvent) {
        return processKeyEvent(keyEvent);
    } else if(wheelEvent) {
        return processWheelEvent(wheelEvent);
    } else if(mouseEvent) {
        return processMouseEvent(mouseEvent);
    }
    return "";
}
//------------------------------------------------------------------------------
QString ShortcutBuilder::processWheelEvent(QWheelEvent *event) {
    QString sequence;
    if(event->angleDelta().ry() < 0) {
        sequence = "WheelUp";
    } else if(event->angleDelta().ry() > 0) {
        sequence = "WheelDown";
    }
    sequence.prepend(modifierKeys(event));
    return sequence;
}
//------------------------------------------------------------------------------
// Detects mouse button clicks only
// DoubleClick works only for LMB
// Otherwise treated as a regular click
QString ShortcutBuilder::processMouseEvent(QMouseEvent *event) {
    QString sequence;
    if(event->button() == Qt::LeftButton)
        sequence = "LMB";
    else if(event->button() == Qt::RightButton)
        sequence = "RMB";
    else if(event->button() == Qt::MiddleButton)
        sequence = "MiddleButton";
    else if(event->button() == Qt::XButton1)
        sequence = "XButton1";
    else if(event->button() == Qt::XButton2)
        sequence = "XButton2";

    sequence.prepend(modifierKeys(event));
    if(event->type() == QEvent::MouseButtonDblClick)
    {
        sequence.append("_DoubleClick");
        return sequence;
    } else if((event->type() == QEvent::MouseButtonPress   && event->button() != Qt::MiddleButton) ||
              (event->type() == QEvent::MouseButtonRelease && event->button() == Qt::MiddleButton))
    {
        // Press event for middle button is reserved for zooming, use release instead
        return sequence;
    }
    return "";
}
//------------------------------------------------------------------------------
QString ShortcutBuilder::processKeyEvent(QKeyEvent *event) {
    //qDebug() << event->nativeScanCode();
    QString sequence;
    if(event->type() == QEvent::KeyPress) {
        sequence = inputMap->keys().value(event->nativeScanCode());
        if(!sequence.isEmpty()) {
            sequence.prepend(modifierKeys(event));
        }
    }
    return sequence;
}
//------------------------------------------------------------------------------
QString ShortcutBuilder::modifierKeys(QEvent *event){
    QKeyEvent   *keyEvent   = dynamic_cast<QKeyEvent *>(event);
    QMouseEvent *mouseEvent = dynamic_cast<QMouseEvent *>(event);
    QWheelEvent *wheelEvent = dynamic_cast<QWheelEvent *>(event);
    QString mods;
    QMapIterator<QString, Qt::KeyboardModifier> i(inputMap->modifiers());
    while(i.hasNext()) {
        i.next();
        if(keyEvent) {
            if(keyEvent->modifiers().testFlag(i.value())) {
                mods.append(i.key());
            }
        } else if(wheelEvent){
            if(wheelEvent->modifiers().testFlag(i.value())) {
                mods.append(i.key());
            }
        } else if(mouseEvent) {
            if(mouseEvent->modifiers().testFlag(i.value())) {
                mods.append(i.key());
            }
        }
    }
    return mods;
}
//------------------------------------------------------------------------------
