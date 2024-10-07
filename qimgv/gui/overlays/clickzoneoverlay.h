#pragma once

#include <QHBoxLayout>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QDebug>
#include "gui/customwidgets/floatingwidget.h"
#include "utils/imagelib.h"

enum ActiveHighlightZone {
    HIGHLIGHT_NONE,
    HIGHLIGHT_LEFT,
    HIGHLIGHT_RIGHT
};

class ClickZoneOverlay : public FloatingWidget
{
    Q_OBJECT
public:
    explicit ClickZoneOverlay(FloatingWidgetContainer *parent);
    QRect leftZone();
    QRect rightZone();
    void highlightLeft();
    void highlightRight();
    void disableHighlight();
    void setHighlightedZone(ActiveHighlightZone zone);
    bool isHighlighted();
    void setPressed(bool mode);

public slots:
    void readSettings();

private:
    QPixmap* loadPixmap(QString path);
    QPixmap *pixmapLeft = nullptr, *pixmapRight = nullptr;
    QRect mLeftZone, mRightZone;
    qreal dpr, pixmapDrawScale;
    bool hiResPixmaps = false;
    const int zoneSize = 110;
    bool isPressed = false;
    bool leftHovered = false, rightHovered = false;
    bool drawZones = true;
    ActiveHighlightZone activeZone = HIGHLIGHT_NONE;
    void drawPixmap(QPainter &p, QPixmap *pixmap, QRect rect);

protected:
    virtual void resizeEvent(QResizeEvent *event);
    virtual void paintEvent(QPaintEvent *event);
    virtual void recalculateGeometry();
};
