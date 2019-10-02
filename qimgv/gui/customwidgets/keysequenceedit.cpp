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
    QString tmp = ShortcutBuilder::fromEvent(e);
    if(!tmp.isEmpty()) {
        mSequence = tmp;
        this->setText(mSequence);
        emit edited();
    }
}
