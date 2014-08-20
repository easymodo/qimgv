#ifndef CONTROLSOVERLAY_H
#define CONTROLSOVERLAY_H

#include <QWidget>
#include <QPainter>
#include <QPen>
#include <QDebug>
#include <QMouseEvent>

class controlsOverlay : public QWidget
{
    Q_OBJECT
public:
    explicit controlsOverlay(QWidget *parent = 0);
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

#endif // CONTROLSOVERLAY_H
