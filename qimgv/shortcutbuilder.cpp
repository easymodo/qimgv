#include "shortcutbuilder.h"

QString ShortcutBuilder::fromEvent(QInputEvent *event) {
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

    if(event->type() == QEvent::MouseButtonDblClick) {
        sequence.append("_DoubleClick");
        return sequence;
    }
    if((event->type() == QEvent::MouseButtonPress   && event->button() != Qt::RightButton) ||
       (event->type() == QEvent::MouseButtonRelease && event->button() == Qt::RightButton))
    {
        return sequence;
    }

    return "";
}
//------------------------------------------------------------------------------
QString ShortcutBuilder::processKeyEvent(QKeyEvent *event) {
#if defined(__linux__) || defined(win32)
    const bool useNativeScanCodes = true;
#else
    const bool useNativeScanCodes = false;
#endif
    // ignore event if the key itself is a modifier
    if(isModifier(Qt::Key(event->key())))
        return "";
    QString sequence;
    if(event->type() == QEvent::KeyPress) {
        if(useNativeScanCodes) {
            // layout-independent method
            // -------------------------
            sequence = inputMap->keys().value(event->nativeScanCode());
        } else {
            // layout-dependent method
            // -------------------------
            // this works for standard keys (numbers, CAPS, Enter etc.)
            // as well as for char keys typed in english layout
            sequence = QVariant::fromValue(Qt::Key(event->key())).toString();
            if(!sequence.isEmpty()) {
                // remove "Key_" at the beginning
                sequence.remove(0,4);
            } else {
                // got an unknown key (usually something from non-eng layout)
                // use it's text value instead
                sequence = QKeySequence(event->key()).toString();
            }
        }
        if(!sequence.isEmpty()) {
            sequence.prepend(modifierKeys(event));
        }
    }
    return sequence;
}
//------------------------------------------------------------------------------
QString ShortcutBuilder::modifierKeys(QInputEvent *event){
    QString mods;
    QMapIterator<QString, Qt::KeyboardModifier> i(inputMap->modifiers());
    while(i.hasNext()) {
        i.next();
        if(event->modifiers().testFlag(i.value()))
            mods.append(i.key());
    }
    return mods;
}
//------------------------------------------------------------------------------
bool ShortcutBuilder::isModifier(Qt::Key key) {
    if(key == Qt::Key_Control ||
       key == Qt::Key_Super_L ||
       key == Qt::Key_Super_R ||
       key == Qt::Key_AltGr   ||
       key == Qt::Key_Shift   ||
       key == Qt::Key_Meta    ||
       key == Qt::Key_Alt     )
    {
        return true;
    }
    return false;
}
