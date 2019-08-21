#ifndef ACTIONBUTTON_H
#define ACTIONBUTTON_H

#include <QPushButton>
#include <QStyleOption>
#include <QPainter>
#include "components/actionmanager/actionmanager.h"

enum TriggerMode {
    PressTrigger,
    ClickTrigger
};

class ActionButton : public QPushButton {
public:
    ActionButton(QWidget *parent = nullptr);
    ActionButton(QString _actionName, QString _iconPath, QWidget *parent = nullptr);
    ActionButton(QString _actionName, QString _iconPath, int _size, QWidget *parent = nullptr);
    void setAction(QString _actionName);

    void setIcon(QIcon icon);
    void setTriggerMode(TriggerMode mode);
    TriggerMode triggerMode();
protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    QString actionName;
    TriggerMode mTriggerMode;
};

#endif // ACTIONBUTTON_H
