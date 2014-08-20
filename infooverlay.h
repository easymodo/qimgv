#ifndef INFOOVERLAY_H
#define INFOOVERLAY_H

#include <QWidget>
#include <QPainter>
#include <QPen>

class infoOverlay : public QWidget
{
    Q_OBJECT
public:
    infoOverlay(QWidget *parent);
    void setText(QString text);
protected:
    void paintEvent(QPaintEvent *event);
    QString currentText;
};

#endif // INFOOVERLAY_H
