#include "keysequenceedit.h"

KeySequenceEdit::KeySequenceEdit(QWidget *parent) : QPushButton(parent), mSequence("") {
    setText("[Enter sequence]");
    connect(this, SIGNAL(pressed()), this, SLOT(enterEditMode()));
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

void KeySequenceEdit::wheelEvent(QWheelEvent *e) {
    processEvent(e);
}

void KeySequenceEdit::processEvent(QEvent *e) {
    mSequence = ShortcutBuilder::fromEvent(e);
    if(!mSequence.isEmpty()) {
        this->setText(mSequence);
        emit edited();
    }
}

void KeySequenceEdit::enterEditMode() {

}

void KeySequenceEdit::exitEditMode() {

}


