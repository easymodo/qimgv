#ifndef MAPOVERLAY_H
#define MAPOVERLAY_H

#include <QWidget>
#include <QPainter>
#include <QPen>
#include <QDebug>


class mapOverlay : public QWidget
{
    Q_OBJECT
public:
    explicit mapOverlay(QWidget *parent = 0);
    void updateMap(QSize scr, QSize img, int srcx, int scry);
    void updateSize();

protected:
    void paintEvent(QPaintEvent *event);
    QPen pen;
    QRect outer,inner;

signals:

public slots:

};

#endif // MAPOVERLAY_H
