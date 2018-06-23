#ifndef VIDEOSLIDER_H
#define VIDEOSLIDER_H

#include <QSlider>
#include <QMouseEvent>
#include <QStyle>
#include <QStyleOptionSlider>

class VideoSlider : public QSlider {
public:
    VideoSlider(QWidget *parent = nullptr);
protected:
    void mousePressEvent(QMouseEvent *event);
};

#endif // VIDEOSLIDER_H
