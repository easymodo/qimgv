#ifndef THUMBNAILLABEL_H
#define THUMBNAILLABEL_H

#include <QLabel>
#include <QPainter>
#include <QMouseEvent>
#include <QDebug>

enum loadState { EMPTY, LOADING, LOADED };

class ThumbnailLabel : public QLabel
{
    Q_OBJECT
public:
    ThumbnailLabel();
    ~ThumbnailLabel();

    QRect relativeRect();
    bool isLoaded();
    loadState state;

public slots:
    void setPixmap(const QPixmap &pixmap);
private:
    bool hovered, loaded;
    QWidget *parent;

signals:
 void clicked(ThumbnailLabel*);

protected:
 void mousePressEvent(QMouseEvent* event);
 void paintEvent(QPaintEvent* event);

 void enterEvent(QEvent *event);
 void leaveEvent(QEvent *event);
};

#endif // THUMBNAILLABEL_H
