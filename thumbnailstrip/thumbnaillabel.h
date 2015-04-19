#ifndef THUMBNAILLABEL_H
#define THUMBNAILLABEL_H

#include <QLabel>
#include <QPainter>
#include <QMouseEvent>

class ThumbnailLabel : public QLabel
{
    Q_OBJECT
public:
    ThumbnailLabel();
    ~ThumbnailLabel();

private:
    bool hovered;

signals:
 void clicked(QLabel*);

protected:
 void mousePressEvent ( QMouseEvent * event ) ;
 virtual void paintEvent(QPaintEvent* event);

 void enterEvent(QEvent *event);
 void leaveEvent(QEvent *event);
};

#endif // THUMBNAILLABEL_H
