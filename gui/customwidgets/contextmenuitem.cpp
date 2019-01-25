#include "contextmenuitem.h"

ContextMenuItem::ContextMenuItem(QWidget *parent)
    : QWidget(parent),
      action("")
{
    layout.setContentsMargins(11,5,11,5);
    layout.setSpacing(7);

    setAccessibleName("ContextMenuItem");
    this->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    icon.setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
    text.setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    shortcut.setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    icon.setMinimumSize(16, 16);

    spacer = new QSpacerItem(16, 1, QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);

    layout.addWidget(&icon);
    layout.addWidget(&text);
    layout.addSpacerItem(spacer);
    layout.addWidget(&shortcut);

    setLayout(&layout);
}

ContextMenuItem::~ContextMenuItem() {
    delete spacer;
}

void ContextMenuItem::setText(QString text) {
    this->text.setText(text);
}

void ContextMenuItem::setPixmap(QPixmap pixmap) {
    this->icon.setPixmap(pixmap);
    this->icon.setFixedSize(pixmap.size());
}

void ContextMenuItem::setIcon(QIcon icon) {
    this->icon.setPixmap(icon.pixmap(16,16));
}

void ContextMenuItem::setAction(QString text) {
    this->action = text;
    shortcut.setText(actionManager->shortcutForAction(action));
}

void ContextMenuItem::onClick() {
    actionManager->invokeAction(action);
}

void ContextMenuItem::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event)
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void ContextMenuItem::mousePressEvent(QMouseEvent *event) {
    // the order here matters
    // hide the parent menu FIRST, then do whatever
    // otherwise it may stay visible when it shouldn't be
    this->parentWidget()->hide();
    onClick();
    QWidget::mousePressEvent(event);
}
