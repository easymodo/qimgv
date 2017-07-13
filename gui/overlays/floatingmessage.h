#ifndef FLOATINGMESSAGE
#define FLOATINGMESSAGE

#include <QTimeLine>
#include <QGraphicsColorizeEffect>
#include <QGraphicsOpacityEffect>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QPainterPath>
#include "gui/overlays/infooverlay.h"

namespace Message {
    enum Position {
        POSITION_LEFT,
        POSITION_RIGHT,
        POSITION_BOTTOM,
        POSITION_TOP,
        POSITION_TOPLEFT,
        POSITION_TOPRIGHT,
        POSITION_BOTTOMLEFT,
        POSITION_BOTTOMRIGHT
    };
    enum Icon {
        NO_ICON,
        ICON_LEFT_EDGE,
        ICON_RIGHT_EDGE
    };
}


class FloatingMessage : public InfoOverlay {
    Q_OBJECT
    Q_PROPERTY (float opacity READ opacity WRITE setOpacity)
public:
    FloatingMessage(QWidget *parent);
    void showMessage(QString _text, Message::Position _position, Message::Icon _icon, int _duration);

    void setIcon(Message::Icon _icon);
public slots:
    void show();

private:
    Message::Position position;
    Message::Icon icon;
    const int iconSize = 32;

private slots:
    void setOpacity(float opacity);
    float opacity() const;

protected:
    virtual void recalculateGeometry();
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);

    QPropertyAnimation *fadeAnimation;
    QParallelAnimationGroup *animGroup;

    QColor effectColor, borderColor;
    qreal effectStrength, currentOpacity;
    int duration;

    const int marginX = 20;
    const int marginY = 30;

    QPixmap iconLeftEdge, iconRightEdge;
    QPixmap *currentIcon;
    QRect iconRect;
};

#endif // FLOATINGMESSAGE
