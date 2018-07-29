#ifndef ICONBUTTON_H
#define ICONBUTTON_H

#include <QPushButton>
#include <QStyleOption>
#include <QPainter>
#include "components/actionmanager/actionmanager.h"

class IconButton : public QPushButton {
public:
    IconButton(QWidget *parent = nullptr);
    IconButton(QString _actionName, QString _iconPath, QWidget *parent = nullptr);
    IconButton(QString _actionName, QString _iconPath, int _size, QWidget *parent = nullptr);
    void setAction(QString _actionName);

    void setIcon(QIcon icon);
protected:
    void mousePressEvent(QMouseEvent *event);
    QString actionName;
};

#endif // ICONBUTTON_H
