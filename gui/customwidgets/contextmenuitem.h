// A menu item that executes an action on press

#ifndef CONTEXTMENUITEM_H
#define CONTEXTMENUITEM_H

#include <QLabel>
#include <QStyleOption>
#include <QHBoxLayout>
#include <QSpacerItem>
#include "components/actionmanager/actionmanager.h"

class ContextMenuItem : public QWidget {
    Q_OBJECT
public:
    ContextMenuItem(QWidget *parent = nullptr);
    ~ContextMenuItem();
    void setText(QString mText);
    QString text();
    void setShortcutText(QString mText);
    QString shortcut();
    void setPixmap(QPixmap pixmap);
    void setIcon(QIcon mIcon);
    void setAction(QString mAction);

protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);

private slots:
    void onClick();

private:
    QLabel mIcon, mText, mShortcut;
    QSpacerItem *spacer;
    QHBoxLayout mLayout;
    QString mAction;
};

#endif // CONTEXTMENUITEM_H
