#ifndef SHORTCUTBUILDER_H
#define SHORTCUTBUILDER_H

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
};

#endif // SHORTCUTBUILDER_H
