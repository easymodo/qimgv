// A MenuItem that simply emits a signal when clicked, with no
// associated file/directory. Used for menu rows that trigger an
// action rather than select a path (e.g. switching the list source).

#pragma once

#include "gui/customwidgets/menuitem.h"

class ActionMenuItem : public MenuItem {
    Q_OBJECT
public:
    ActionMenuItem(QWidget *parent);

protected:
    void onPress() override;

signals:
    void activated();
};
