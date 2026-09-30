#include "actionmenuitem.h"

ActionMenuItem::ActionMenuItem(QWidget *parent)
    : MenuItem(parent)
{
    setFocusPolicy(Qt::NoFocus);
}

void ActionMenuItem::onPress() {
    emit activated();
}
