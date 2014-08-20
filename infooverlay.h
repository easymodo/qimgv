#ifndef INFOOVERLAY_H
#define INFOOVERLAY_H

#include <QWidget>
#include <QPainter>
#include <QPen>
#include <QDebug>

class infoOverlay : public QWidget
{
    Q_OBJECT
public:
    infoOverlay(QWidget *parent);
    void setText(QString text);
    void updateSize();

protected:
    void paintEvent(QPaintEvent *event);
    QString currentText;
};

#endif // INFOOVERLAY_H
