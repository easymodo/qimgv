#include "menuitem.h"

MenuItem::MenuItem(QWidget *parent)
    : QWidget(parent)
{
    //mLayout.setContentsMargins(11,5,11,5);
    mLayout.setContentsMargins(6,0,11,0);
    mLayout.setSpacing(3);

    setAccessibleName("MenuItem");
    this->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    mIconLabel.setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
    mTextLabel.setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    mShortcutLabel.setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    mIconLabel.setMinimumSize(16, 16);

    spacer = new QSpacerItem(16, 1, QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    mIconLabel.setAccessibleName("MenuItemIcon");
    mTextLabel.setAccessibleName("MenuItemText");
    mShortcutLabel.setAccessibleName("MenuItemShortcutLabel");
    mLayout.addWidget(&mIconLabel);
    mLayout.addWidget(&mTextLabel);
    mLayout.addSpacerItem(spacer);
    mLayout.addWidget(&mShortcutLabel);

    setLayout(&mLayout);
}

MenuItem::~MenuItem() {
    delete spacer;
}

void MenuItem::setText(QString text) {
    this->mTextLabel.setText(text);
}

QString MenuItem::text() {
    return mTextLabel.text();
}

void MenuItem::setShortcutText(QString text) {
    this->mShortcutLabel.setText(text);
}

QString MenuItem::shortcut() {
    return mShortcutLabel.text();
}

void MenuItem::setPixmap(QPixmap pixmap) {
    this->mIconLabel.setPixmap(pixmap);
    this->mIconLabel.setFixedSize(pixmap.size());
}

void MenuItem::setIcon(QIcon icon) {
    this->mIconLabel.setPixmap(icon.pixmap(16,16));
}

void MenuItem::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event)
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void MenuItem::onPress() {
}

void MenuItem::onClick() {
}

void MenuItem::mousePressEvent(QMouseEvent *event) {
    onPress();
    QWidget::mousePressEvent(event);
}

void MenuItem::mouseReleaseEvent(QMouseEvent *event) {
    onClick();
    QWidget::mouseReleaseEvent(event);
}
