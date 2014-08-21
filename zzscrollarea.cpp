#include "zzscrollarea.h"

zzScrollArea::zzScrollArea(QWidget *parent) :
    QScrollArea(parent)
{
    this->horizontalScrollBar()->setStyleSheet("QScrollBar {height:0px;}");
    this->verticalScrollBar()->setStyleSheet("QScrollBar {width:0px;}");
}

void zzScrollArea::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        this->setCursor(QCursor(Qt::ClosedHandCursor));
        lastDragPos = event->pos();
    }
}

void zzScrollArea::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        this->setCursor(QCursor(Qt::ArrowCursor));
        lastDragPos = event->pos();
    }
}

void zzScrollArea::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton)
    {
        temp = lastDragPos - event->pos();
        if( (temp.rx()+this->horizontalScrollBar()->value()) > this->horizontalScrollBar()->maximum() ) {
            this->horizontalScrollBar()->setValue(this->horizontalScrollBar()->maximum());
        }
        else {
            this->horizontalScrollBar()->setValue(this->horizontalScrollBar()->value()+temp.rx());
        }

        if( (temp.ry()+this->verticalScrollBar()->value()) > this->verticalScrollBar()->maximum() ) {
            this->verticalScrollBar()->setValue(this->verticalScrollBar()->maximum());
        }
        else {
            this->verticalScrollBar()->setValue(this->verticalScrollBar()->value()+temp.ry());
        }
        lastDragPos = event->pos();
        emit scrollbarChanged();
    }
}

void zzScrollArea::mouseDoubleClickEvent(QMouseEvent *event) {
    emit sendDoubleClick();
}

void zzScrollArea::wheelEvent(QWheelEvent *event) {
    event->ignore();
}

void zzScrollArea::resizeEvent(QResizeEvent *event) {
    QScrollArea::resizeEvent(event);
    emit resized();
}

void zzScrollArea::keyPressEvent(QKeyEvent *event) {
    event->ignore();
}

