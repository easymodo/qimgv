#ifndef INFOOVERLAY_H
#define INFOOVERLAY_H

#include "../customWidgets/overlaywidget.h"
#include <QPainter>
#include <QPen>
#include <QFontMetrics>
#include <QDebug>

class InfoOverlay : public OverlayWidget
{
    Q_OBJECT
public:
    InfoOverlay(QWidget *parent);
    void setText(QString text);

private:
    QFont font;
    QRect textRect;
    QColor textColor, textShadowColor, bgColor;
    int textMarginX, textMarginY;
    QFontMetrics *fm;

protected:
    virtual void recalculateGeometry();
    void paintEvent(QPaintEvent *event);
    QString text;
};

#endif // INFOOVERLAY_H
