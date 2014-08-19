#include "zzscrollarea.h"

zzScrollArea::zzScrollArea(QWidget *parent) :
    QScrollArea(parent)
{
    //this->
}


void zzScrollArea::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        this->setCursor(QCursor(Qt::ClosedHandCursor));
    //    lastDragPos = event->pos();
    }
}

void zzScrollArea::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        this->setCursor(QCursor(Qt::ArrowCursor));
    //    lastDragPos = event->pos();
    }
}

void zzScrollArea::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton)
    {
        //qDebug() << lastDragPos - event->pos();
        temp = -1*(lastDragPos - event->pos());
        qDebug() << temp;
        this->scroll(temp.rx(),temp.ry());
        //lastDragPos = event->pos();
       // this->horizontalScrollBar()->setValue(100);
    }
}
