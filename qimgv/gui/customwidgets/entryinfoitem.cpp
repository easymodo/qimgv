#include "entryinfoitem.h"

EntryInfoItem::EntryInfoItem(QWidget *parent) : QWidget(parent) {
    layout.setContentsMargins(9,2,9,2);
    layout.setSpacing(6);
    layout.addWidget(&nameLabel);
    layout.addWidget(&valueLabel, 1);
    layout.setAlignment(&nameLabel, Qt::AlignTop);
    layout.setAlignment(&valueLabel, Qt::AlignTop);
    setLayout(&layout);

    valueLabel.setFixedWidth(142);
    valueLabel.setWordWrap(true);

    // add some padding for easier text selection
    valueLabel.setContentsMargins(3,0,0,0);
    valueLabel.setTextInteractionFlags(Qt::TextSelectableByMouse);
    valueLabel.setCursor(Qt::IBeamCursor);
}

void EntryInfoItem::setInfo(QString _name, QString _value) {
    name = _name;
    value = _value;
    nameLabel.setText(name);
    valueLabel.setText(value);
};

void EntryInfoItem::setNameColumnWidth(int width) {
    nameLabel.setFixedWidth(width);
}

int EntryInfoItem::nameWidthHint() const {
    return nameLabel.fontMetrics().horizontalAdvance(name);
}

void EntryInfoItem::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event)
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
