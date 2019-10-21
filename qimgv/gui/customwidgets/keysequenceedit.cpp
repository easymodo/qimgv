#include "keysequenceedit.h"

KeySequenceEdit::KeySequenceEdit(QWidget *parent) : QPushButton(parent), mSequence("") {
    setText(tr("[Enter sequence]"));
}

QString KeySequenceEdit::sequence() {
    return mSequence;
}

void KeySequenceEdit::keyPressEvent(QKeyEvent *e) {
    processEvent(e);
}

void KeySequenceEdit::mousePressEvent(QMouseEvent *e) {
    processEvent(e);
}

void KeySequenceEdit::mouseReleaseEvent(QMouseEvent *e) {
    processEvent(e);
}

void KeySequenceEdit::wheelEvent(QWheelEvent *e) {
    processEvent(e);
}

void KeySequenceEdit::processEvent(QEvent *e) {
    QInputEvent *inputEvent = dynamic_cast<QInputEvent *>(e);
    if(!inputEvent)
        return;
    QString tmp = ShortcutBuilder::fromEvent(inputEvent);
    if(!tmp.isEmpty()) {
        mSequence = tmp;
        this->setText(mSequence);
        emit edited();
    }
}
