// Base class for various menu items.
// Displays entry name, shortcut and an icon.

#ifndef MENUITEM_H
#define MENUITEM_H

#include <QLabel>
#include <QStyleOption>
#include <QHBoxLayout>
#include <QSpacerItem>
#include "gui/customwidgets/clickablelabel.h"
#include "components/actionmanager/actionmanager.h"

class MenuItem : public QWidget {
    Q_OBJECT
public:
    MenuItem(QWidget *parent = nullptr);
    ~MenuItem();
    void setText(QString mTextLabel);
    QString text();
    void setShortcutText(QString mTextLabel);
    QString shortcut();
    void setPixmap(QPixmap pixmap);
    void setIcon(QIcon mIconLabel);

protected:
    ClickableLabel mIconLabel;
    QLabel mTextLabel, mShortcutLabel;
    QSpacerItem *spacer;
    QHBoxLayout mLayout;
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    virtual void onClick();
    virtual void onPress();
};

#endif // MENUITEM_H
