#ifndef INFOOVERLAY_H
#define INFOOVERLAY_H

#include <QWidget>
#include <QPainter>
#include <QPen>
#include <QDebug>

enum Position {
    TOP,
    BOTTOM
};

class textOverlay : public QWidget
{
    Q_OBJECT
public:
    textOverlay(QWidget *parent, Position);
    void setText(QString text);
    void updateWidth();

public slots:
    void updatePosition();

protected:
    void paintEvent(QPaintEvent *event);
    Position position;
    QString currentText;
};

#endif // INFOOVERLAY_H
