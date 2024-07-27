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
    if(event->angleDelta() == QPoint(0,0))
        return "";
    if(event->angleDelta().ry() < 0)
        sequence = "WheelDown";
    else if(event->angleDelta().ry() > 0)
        sequence = "WheelUp";
    if(event->angleDelta().rx() < 0) // fallback to horizontal
        sequence = "WheelDown";
    else if(event->angleDelta().rx() > 0)
        sequence = "WheelUp";
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
    if(event->type() != QEvent::KeyPress || isModifier(Qt::Key(event->key())))
        return "";
#if defined(__linux__) || defined(__FreeBSD__) || defined(_WIN32)
    return fromEventNativeScanCode(event);
#else
    return fromEventText(event);
#endif
}
//------------------------------------------------------------------------------
QString ShortcutBuilder::modifierKeys(QInputEvent *event){
    QString mods;
    QMapIterator<QString, Qt::KeyboardModifier> i(inputMap->modifiers());
    while(i.hasNext()) {
        i.next();
        if(event->modifiers().testFlag(i.value()))
            mods.append(i.key() + "+");
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
//------------------------------------------------------------------------------
QString ShortcutBuilder::fromEventNativeScanCode(QKeyEvent *event) {
    // layout-independent method (mostly)
    // -------------------------
    // Still has some issues like when you use two layouts
    // where on the same button you have a letter on one layout and some symbol on the other.
    // I'm leaving this as-is because trying to fix all layouts will turn into a mess real quick.
    // You can always just add the same keybind using your alt. layout if it doesnt work.
    QString sequence = inputMap->keys().value(event->nativeScanCode());

    if(sequence.isEmpty())
        return sequence;

    QString eventText = event->text();

    QChar keyChr = eventText.isEmpty() ? QChar() : eventText.at(0);

    // use alt characters accessed by shift (punctuation on the numbers row etc.)
    bool useAltChr = (event->modifiers() == Qt::ShiftModifier) && !(keyChr.isLetter() || !keyChr.isPrint() || keyChr.isSpace());
    if(useAltChr) {
        sequence = eventText;
    } else if(!sequence.isEmpty()) {
        sequence.prepend(modifierKeys(event));
    }

    //qDebug() << "RESULT:" << sequence;
    return sequence;
}
//------------------------------------------------------------------------------
QString ShortcutBuilder::fromEventText(QKeyEvent *event) {
    // layout-dependent method
    // -------------------------
    // Works on platforms for which there is no native scancode support from Qt.
    // Keybinds will work only on the same layout they were added (except non-printables).
    QString sequence = QVariant::fromValue(Qt::Key(event->key())).toString();
    if(!sequence.isEmpty()) {
        // remove "Key_" at the beginning
        sequence.remove(0,4);
        // rename some keys to match the ones from inputmap
        // just a bandaid
        if(sequence == "Return")
            sequence = "Enter";
        else if(sequence == "Escape")
            sequence = "Esc";
    } else {
        // got an unknown key (usually something from non-eng layout)
        // use it's text value instead
        sequence = QKeySequence(event->key()).toString();
    }
    if(!sequence.isEmpty())
        sequence.prepend(modifierKeys(event));
    return sequence;
}
