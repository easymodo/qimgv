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
    void setText(QString text);
    void setPixmap(QPixmap pixmap);
    void setIcon(QIcon icon);
    void setAction(QString action);

protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);

private slots:
    void onClick();

private:
    QLabel icon, text, shortcut;
    QSpacerItem *spacer;
    QHBoxLayout layout;
    QString action;
};

#endif // CONTEXTMENUITEM_H
