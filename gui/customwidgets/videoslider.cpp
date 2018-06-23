#include "videoslider.h"

VideoSlider::VideoSlider(QWidget *parent) : QSlider(parent) {

}

void VideoSlider::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton)
        {
            if (orientation() == Qt::Vertical)
                setValue(minimum() + ((maximum()-minimum()) * (height()-event->y())) / height() ) ;
            else
                setValue(minimum() + ((maximum()-minimum()) * event->x()) / width() ) ;

            event->accept();
        }
        QSlider::mousePressEvent(event);
}
