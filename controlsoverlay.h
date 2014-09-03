#ifndef ControlsOverlay_H
#define ControlsOverlay_H

#include <QWidget>
#include <QPainter>
#include <QPen>
#include <QDebug>
#include <QMouseEvent>

class ControlsOverlay : public QWidget
{
    Q_OBJECT
public:
    explicit ControlsOverlay(QWidget *parent = 0);
    void updateSize();

protected:
    void paintEvent(QPaintEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    QPen pen;
    
signals:
    void exitClicked();
    void exitFullscreenClicked();
    void minimizeClicked();
    
public slots:
};

#endif // ControlsOverlay_H
