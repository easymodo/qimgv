#ifndef CONTEXTMENUITEM_H
#define CONTEXTMENUITEM_H

#include "gui/customwidgets/clickablewidget.h"
#include <QLabel>
#include <QStyleOption>
#include <QHBoxLayout>
#include <QSpacerItem>
#include "components/actionmanager/actionmanager.h"

class ContextMenuItem : public ClickableWidget {
    Q_OBJECT
public:
    ContextMenuItem(QWidget *parent = nullptr);
    ~ContextMenuItem();
    void setText(QString text);
    void setPixmap(QPixmap pixmap);
    void setAction(QString action);

protected:
    void paintEvent(QPaintEvent *event);

private slots:
    void onClick();

private:
    QLabel icon, text, shortcut;
    QSpacerItem *spacer;
    QHBoxLayout layout;
    QString action;
};

#endif // CONTEXTMENUITEM_H
