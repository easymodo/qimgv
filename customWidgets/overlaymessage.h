#ifndef OVERLAYMESSAGE_H
#define OVERLAYMESSAGE_H

#include "../overlays/infooverlay.h"
#include <QTimeLine>
#include <QGraphicsColorizeEffect>
#include <QGraphicsOpacityEffect>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>

enum MessagePosition {
    MESSAGE_LEFT,
    MESSAGE_RIGHT,
    MESSAGE_BOTTOM,
    MESSAGE_TOP,
    MESSAGE_TOPLEFT,
    MESSAGE_TOPRIGHT,
    MESSAGE_BOTTOMLEFT,
    MESSAGE_BOTTOMRIGHT
};

class OverlayMessage : public InfoOverlay {
    Q_OBJECT
    Q_PROPERTY (float opacity READ opacity WRITE setOpacity)
public:
    OverlayMessage(QWidget *parent);
    void showMessage(QString _text, MessagePosition _position, int _duration);

public slots:
    void show();

private:
    MessagePosition position;

private slots:
    void setOpacity(float opacity);
    float opacity() const;

protected:
    virtual void recalculateGeometry();
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);

    QPropertyAnimation *colorizeAnimation, *fadeAnimation;
    QGraphicsColorizeEffect *colorizeEffect;
    QParallelAnimationGroup *animGroup;

    QColor effectColor, borderColor;
    qreal effectStrength, currentOpacity;
    int duration;

    const int marginX = 25;
    const int marginY = 40;
};

#endif // OVERLAYMESSAGE_H
