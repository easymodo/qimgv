#ifndef INFOOVERLAY_H
#define INFOOVERLAY_H

#include <QWidget>
#include <QPainter>
#include <QPen>
#include <QFontMetrics>
#include <QDebug>

class textOverlay : public QWidget
{
    Q_OBJECT
public:
    textOverlay(QWidget *parent);
    void setText(QString text);
    void updateWidth(int maxWidth);

private:
    QFont font;
    QRect drawRect;
    QColor *textColor, *textShadowColor;
    int textLength;
    QFontMetrics *fm;

public slots:
    void updatePosition();

protected:
    void paintEvent(QPaintEvent *event);
    QString currentText;
};

#endif // INFOOVERLAY_H
