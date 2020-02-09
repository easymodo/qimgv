#pragma once

#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QDebug>
#include "utils/inputmap.h"

class ShortcutBuilder {
public:
    static QString fromEvent(QInputEvent *event);

private:
    static QString processWheelEvent(QWheelEvent *event);
    static QString processMouseEvent(QMouseEvent *event);
    static QString processKeyEvent(QKeyEvent *event);
    static QString modifierKeys(QInputEvent *event);
    static bool isModifier(Qt::Key key);
    static QString fromEventText(QKeyEvent *event);
    static QString fromEventNativeScanCode(QKeyEvent *event);
};
