#pragma once

#include <QTimer>
#include <QLabel>
#include "gui/customwidgets/overlaywidget.h"
#include "settings.h"

namespace Ui {
class FloatingMessage;
}

enum FloatingMessageIcon {
    NO_ICON,
    ICON_DIRECTORY,
    ICON_LEFT_EDGE,
    ICON_RIGHT_EDGE,
    ICON_SUCCESS,
    ICON_WARNING,
    ICON_ERROR
};

class FloatingMessage : public OverlayWidget {
    Q_OBJECT 
public:
    FloatingMessage(FloatingWidgetContainer *parent);
    ~FloatingMessage();
    void showMessage(QString text, FloatingMessageIcon icon, int fadeDuration);
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
    void setIcon(FloatingMessageIcon icon);

protected:
    void mousePressEvent(QMouseEvent *event);

private slots:
    void readSettings();
};
