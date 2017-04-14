#include "clickablelabel.h"

ClickableLabel::ClickableLabel() {
}

ClickableLabel::ClickableLabel(QWidget *parent) : QLabel(parent) {
}

ClickableLabel::ClickableLabel(const QString &text) {
    this->setText(text);
}

ClickableLabel::ClickableLabel(const QString &text, QWidget *parent) : QLabel(parent) {
    this->setText(text);
}

void ClickableLabel::mousePressEvent(QMouseEvent *event) {
    Q_UNUSED (event)
    emit clicked();
}
