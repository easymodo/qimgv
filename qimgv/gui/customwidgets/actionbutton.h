#pragma once

#include "gui/customwidgets/iconbutton.h"
#include "components/actionmanager/actionmanager.h"

enum TriggerMode {
    PressTrigger,
    ClickTrigger
};

class ActionButton : public IconButton {
public:
    ActionButton(QWidget *parent = nullptr);
    ActionButton(QString _actionName, QString _iconPath, QWidget *parent = nullptr);
    ActionButton(QString _actionName, QString _iconPath, int _size, QWidget *parent = nullptr);
    void setAction(QString _actionName);
    void setTriggerMode(TriggerMode mode);
    TriggerMode triggerMode();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    QString actionName;
    TriggerMode mTriggerMode;
};
