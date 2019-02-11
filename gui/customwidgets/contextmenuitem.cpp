#include "contextmenuitem.h"

ContextMenuItem::ContextMenuItem(QWidget *parent)
    : QWidget(parent),
      mAction("")
{
    mLayout.setContentsMargins(11,5,11,5);
    mLayout.setSpacing(7);

    setAccessibleName("ContextMenuItem");
    this->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    mIcon.setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
    mText.setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    mShortcut.setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    mIcon.setMinimumSize(16, 16);

    spacer = new QSpacerItem(16, 1, QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);

    mLayout.addWidget(&mIcon);
    mLayout.addWidget(&mText);
    mLayout.addSpacerItem(spacer);
    mLayout.addWidget(&mShortcut);

    setLayout(&mLayout);
}

ContextMenuItem::~ContextMenuItem() {
    delete spacer;
}

void ContextMenuItem::setText(QString text) {
    this->mText.setText(text);
}

QString ContextMenuItem::text() {
    return mText.text();
}

void ContextMenuItem::setShortcutText(QString text) {
    this->mShortcut.setText(text);
}

QString ContextMenuItem::shortcut() {
    return mShortcut.text();
}

void ContextMenuItem::setPixmap(QPixmap pixmap) {
    this->mIcon.setPixmap(pixmap);
    this->mIcon.setFixedSize(pixmap.size());
}

void ContextMenuItem::setIcon(QIcon icon) {
    this->mIcon.setPixmap(icon.pixmap(16,16));
}

void ContextMenuItem::setAction(QString text) {
    this->mAction = text;
    setShortcutText(actionManager->shortcutForAction(mAction));
}

void ContextMenuItem::onClick() {
    if(!mAction.isEmpty())
        actionManager->invokeAction(mAction);
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
    //this->parentWidget()->hide();
    onClick();
    QWidget::mousePressEvent(event);
}
