#ifndef FLOATINGMESSAGE
#define FLOATINGMESSAGE

#include <QTimer>
#include <QLabel>
#include "gui/customwidgets/floatingwidget.h"
#include "settings.h"

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
    void showMessage(QString text, FloatingMessageIcon icon, int fadeDuration);
    void setIcon(FloatingMessageIcon icon);

    void showMessage(QString text, FloatingWidgetPosition position, FloatingMessageIcon icon, int duration);
public slots:
    void show();
    void setText(QString text);

private:
    QTimer visibilityTimer;
    int hideDelay;
    FloatingWidgetPosition preferredPosition;
    Ui::FloatingMessage *ui;

    void doShowMessage(QString text, FloatingMessageIcon icon, int duration);
protected:
    void mousePressEvent(QMouseEvent *event);
    QPixmap iconLeftEdge, iconRightEdge;
private slots:
    void readSettings();
};

#endif // FLOATINGMESSAGE
