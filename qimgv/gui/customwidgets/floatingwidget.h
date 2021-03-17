/*
 * Base class for floating panels / overlay widgets.
 * It is not supposed to go into any kind of layout, just set parent & call show().
 * Usage: reimplement recalculateGeometry() method, which sets the new
 * geometry when the parent is resized.
 */

#pragma once

#include "gui/customwidgets/floatingwidgetcontainer.h"
#include <QStyleOption>
#include <QPainter>
#include <QApplication>
#include <QDebug>

#include <QWheelEvent>

class FloatingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FloatingWidget(FloatingWidgetContainer *parent);
    QSize containerSize();
    bool acceptKeyboardFocus() const;
    void setAcceptKeyboardFocus(bool mode);

public slots:
    void hide();

protected:
    // called whenever container rectangle changes
    // this does nothing, reimplement to use
    virtual void recalculateGeometry();
    void paintEvent(QPaintEvent *event);
    void setContainerSize(QSize container);

    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
private:
    // size of whatever widget we are overlayed on
    QSize container;
    bool mAcceptKeyboardFocus;

private slots:
    void onContainerResized(QSize container);
};
