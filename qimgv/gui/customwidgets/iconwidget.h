#pragma once

#include <QWidget>
#include <QStyleOption>
#include <QPainter>
#include <QDebug>
#include <QMouseEvent>
#include "settings.h"
#include "utils/imagelib.h"

class IconWidget : public QWidget
{
public:
    explicit IconWidget(QWidget *parent = nullptr);
    ~IconWidget();
    void setIconPath(QString path);
    void setIconOffset(int x, int y);

protected:
    void paintEvent(QPaintEvent *event);

private:
    bool hiResPixmap;
    QPoint iconOffset;
    QPixmap *pixmap;
    qreal dpr, pixmapDrawScale;
};
