#pragma once
#include <QPushButton>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include "settings.h"

class PushButtonFocusInd : public QPushButton {
    Q_OBJECT
public:
    PushButtonFocusInd(QWidget *parent = nullptr);
    void setHighlighted(bool);
    bool isHighlighted();

signals:
    void rightPressed();
    void rightClicked();

protected:
    virtual void mousePressEvent(QMouseEvent *event);
    bool mIsHighlighted;
    void paintEvent(QPaintEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
};

