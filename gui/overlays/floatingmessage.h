#ifndef FLOATINGMESSAGE
#define FLOATINGMESSAGE

#include <QTimer>
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
public:
    FloatingMessage(OverlayContainerWidget *parent);
    ~FloatingMessage();
    void showMessage(QString text, FloatingWidgetPosition position, FloatingMessageIcon icon, int fadeDuration);
    void setIcon(FloatingMessageIcon icon);

public slots:
    void show();
    void setText(QString text);

private:
    Ui::FloatingMessage *ui;
    QTimer visibilityTimer;
    int hideDelay;

protected:
    void mousePressEvent(QMouseEvent *event);
    QPixmap iconLeftEdge, iconRightEdge;
};

#endif // FLOATINGMESSAGE
