#ifndef VIDEOSLIDER_H
#define VIDEOSLIDER_H

#include <QSlider>
#include <QMouseEvent>
#include <QStyle>
#include <QStyleOptionSlider>
#include <QDebug>

class VideoSlider : public QSlider {
    Q_OBJECT
public:
    VideoSlider(QWidget *parent = nullptr);
protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
signals:
    void sliderMovedX(int);
private:
    void setValueAtCursor(QPoint pos);
};

#endif // VIDEOSLIDER_H
