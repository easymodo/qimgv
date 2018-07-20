#ifndef ICONBUTTON_H
#define ICONBUTTON_H

#include <QLabel>
#include <QStyleOption>
#include <QPainter>
#include "components/actionmanager/actionmanager.h"

class IconButton : public QLabel {
public:
    IconButton(QWidget *parent = nullptr);
    IconButton(QString actionName, QString iconPath, QWidget *parent = nullptr);
    void setAction(QString _actionName);

protected:
    void mousePressEvent(QMouseEvent *event);
    QString actionName;

};

#endif // ICONBUTTON_H
