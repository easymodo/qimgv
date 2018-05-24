#ifndef FLOATINGMESSAGE
#define FLOATINGMESSAGE

#include <QTimeLine>
#include <QGraphicsOpacityEffect>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QLabel>
#include "gui/customwidgets/floatingwidget.h"

namespace Ui {
class FloatingMessage;
}

enum FloatingMessageIcon {
    NO_ICON,
    ICON_LEFT_EDGE,
    ICON_RIGHT_EDGE
};


class FloatingMessage : public FloatingWidget {
    Q_OBJECT
    Q_PROPERTY (float opacity READ opacity WRITE setOpacity)
public:
    FloatingMessage(OverlayContainerWidget *parent);
    ~FloatingMessage();
    void showMessage(QString text, FloatingWidgetPosition position, FloatingMessageIcon icon, int duration);

    void setIcon(FloatingMessageIcon icon);
public slots:
    void show();
    void setText(QString text);

private:
    Ui::FloatingMessage *ui;
    QGraphicsOpacityEffect *opacityEffect;

private slots:
    void setOpacity(float opacity);
    float opacity() const;

protected:
    void mousePressEvent(QMouseEvent *event);

    QPropertyAnimation *fadeAnimation;
    QParallelAnimationGroup *animGroup;

    qreal effectStrength;
    int duration;
    QPixmap iconLeftEdge, iconRightEdge;
};

#endif // FLOATINGMESSAGE
