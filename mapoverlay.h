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
    void setImageRect(QSize *_img);
    void updateMap(double *xdiff, double *ydiff, double *scrx, double *scry, double *ar);
    void updateSize();

protected:
    void paintEvent(QPaintEvent *event);
    QPen penInner, penOuter;
    QRect *outer,*inner;
    int mapSize, mapMargin;

signals:

public slots:

};

#endif // MAPOVERLAY_H
