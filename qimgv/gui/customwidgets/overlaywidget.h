/* A widget which is overlayed on top of another (usually parent).
 * Usage: implement recalculateGeometry() method, which sets the new
 * geometry depending on containerSize().
 */

#ifndef OVERLAYWIDGET_H
#define OVERLAYWIDGET_H

#include "gui/customwidgets/overlaycontainerwidget.h"
#include <QStyleOption>
#include <QPainter>
#include <QDebug>

class OverlayWidget : public QWidget
{
    Q_OBJECT
public:
    explicit OverlayWidget(OverlayContainerWidget *parent);
    QSize containerSize();

protected:
    // called whenever container rectangle changes
    virtual void recalculateGeometry() = 0;
    void paintEvent(QPaintEvent *event);
    void setContainerSize(QSize container);

private:
    // size of whatever widget we are overlayed on
    QSize container;

private slots:
    void onContainerResized(QSize container);
};

#endif // OVERLAYWIDGET_H
