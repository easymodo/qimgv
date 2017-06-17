#ifndef ICONBUTTON_H
#define ICONBUTTON_H

#include <QLabel>
#include <QStyleOption>
#include <QPainter>
#include "components/actionmanager/actionmanager.h"

class IconButton : public QLabel {
public:
    IconButton();
    IconButton(QString actionName, QString iconPath);
    IconButton(QString actionName, QString iconPath, QWidget *parent);
    void setAction(QString _actionName);

protected:
    void mousePressEvent(QMouseEvent *event);
    QString actionName;

};

#endif // ICONBUTTON_H
