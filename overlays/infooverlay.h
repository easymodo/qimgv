#ifndef INFOOVERLAY_H
#define INFOOVERLAY_H

#include <QWidget>
#include <QPainter>
#include <QPen>
#include <QDebug>

class textOverlay : public QWidget
{
    Q_OBJECT
public:
    textOverlay(QWidget *parent);
    void setText(QString text);
    void updateWidth();

private:
    QFont font;
    QRect drawRect;
    QColor *textColor, *textShadowColor;

public slots:
    void updatePosition();

protected:
    void paintEvent(QPaintEvent *event);
    QString currentText;
};

#endif // INFOOVERLAY_H
