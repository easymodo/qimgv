#ifndef ACTIONBUTTON_H
#define ACTIONBUTTON_H

#include "gui/customwidgets/clickablelabel.h"
#include <QPushButton>
#include <QStyleOption>
#include <QPainter>
#include "components/actionmanager/actionmanager.h"

enum TriggerMode {
    PressTrigger,
    ClickTrigger
};

class ActionButton : public ClickableLabel {
    Q_OBJECT
public:
    ActionButton(QWidget *parent = nullptr);
    ActionButton(QString _actionName, QString _iconPath, QWidget *parent = nullptr);
    ActionButton(QString _actionName, QString _iconPath, int _size, QWidget *parent = nullptr);
    void setAction(QString _actionName);
    void setTriggerMode(TriggerMode mode);
    TriggerMode triggerMode();
    void setCheckable(bool mode);

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    QString actionName;
    TriggerMode mTriggerMode;

signals:
    void toggled(bool);

private:
    bool checkable;
};

#endif // ACTIONBUTTON_H
