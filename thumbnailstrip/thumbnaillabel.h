#ifndef THUMBNAILLABEL_H
#define THUMBNAILLABEL_H

#include <QLabel>
#include <QMouseEvent>

class ThumbnailLabel : public QLabel
{
    Q_OBJECT
public:
    ThumbnailLabel();
    ~ThumbnailLabel();

signals:
 void clicked(QLabel*);

protected:
 void mousePressEvent ( QMouseEvent * event ) ;
};

#endif // THUMBNAILLABEL_H
