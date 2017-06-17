#ifndef INFOOVERLAY_H
#define INFOOVERLAY_H

#include <QPainter>
#include <QPen>
#include <QFontMetrics>
#include <QDebug>
#include "gui/customwidgets/overlaywidget.h"

class InfoOverlay : public OverlayWidget
{
    Q_OBJECT
public:
    InfoOverlay(QWidget *parent);
    void setText(QString text);

    void setFontSize(int sz);

private:

protected:
    virtual void recalculateGeometry();
    void paintEvent(QPaintEvent *event);
    QString text;

    QFont font;
    QRect textRect;
    QColor textColor, textShadowColor, bgColor;
    int textMarginX, textMarginY;
    QFontMetrics *fm;
};

#endif // INFOOVERLAY_H
