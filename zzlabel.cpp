#include "zzlabel.h"

zzLabel::zzLabel(QWidget *parent) :
    QLabel(parent)
{
}

void zzLabel::resizeEvent(QResizeEvent *event) {
    QLabel::resizeEvent(event);
    emit resized();
}
