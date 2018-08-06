#include "spinboxinputfix.h"

SpinBoxInputFix::SpinBoxInputFix(QWidget *parent) : QSpinBox(parent) {
    allowedKeys << "1"
                << "2"
                << "3"
                << "4"
                << "5"
                << "6"
                << "7"
                << "8"
                << "9"
                << "0"
                << "Up"
                << "Down"
                << "Left"
                << "Right"
                << "pageUp"
                << "pageDown"
                << "Return"
                << "home"
                << "end"
                << "delete"
                << "backspace";
}

void SpinBoxInputFix::keyPressEvent(QKeyEvent *event) {
    quint32 nativeScanCode = event->nativeScanCode();
    QString key = actionManager->keyForNativeScancode(nativeScanCode);
    if(allowedKeys.contains(key)) {
        QSpinBox::keyPressEvent(event);
    } else {
        event->ignore();
    }
}
