// Base class for various menu items.
// Displays entry name, shortcut and an icon.

#pragma once

#include <QLabel>
#include <QStyleOption>
#include <QHBoxLayout>
#include <QSpacerItem>
#include "gui/customwidgets/iconbutton.h"
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
    void setIconPath(QString path);
    void setPassthroughClicks(bool mode);

protected:
    IconButton mIconWidget;
    QLabel mTextLabel, mShortcutLabel;
    QSpacerItem *spacer;
    QHBoxLayout *mLayout;
    bool passthroughClicks = true;
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    virtual void onClick();
    virtual void onPress();
};
