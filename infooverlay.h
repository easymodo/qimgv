#ifndef INFOOVERLAY_H
#define INFOOVERLAY_H

#include <QWidget>
#include <QPainter>
#include <QPen>
#include <QDebug>

class InfoOverlay : public QWidget
{
    Q_OBJECT
public:
    InfoOverlay(QWidget *parent);
    void setText(QString text);
    void updateSize();

protected:
    void paintEvent(QPaintEvent *event);
    QString currentText;
};

#endif // INFOOVERLAY_H
