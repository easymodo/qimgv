#include "contextmenuitem.h"

ContextMenuItem::ContextMenuItem(QWidget *parent)
    : MenuItem(parent),
      mAction("")
{
}

ContextMenuItem::~ContextMenuItem() {
}

void ContextMenuItem::setAction(QString text) {
    this->mAction = text;
    setShortcutText(actionManager->shortcutForAction(mAction));
}

void ContextMenuItem::onPress() {
    if(!mAction.isEmpty())
        actionManager->invokeAction(mAction);
}
