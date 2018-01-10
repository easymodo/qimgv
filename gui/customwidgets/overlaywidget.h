/* A widget which is overlayed on top of another (usually parent).
 * Usage: implement recalculateGeometry() method, which sets the new
 * geometry depending on containerSize().
 * Call setContainerSize(QSize) whenever its container is resized.
 */

#ifndef OVERLAYWIDGET_H
#define OVERLAYWIDGET_H

#include <QWidget>
#include <QStyleOption>
#include <QPainter>

class OverlayWidget : public QWidget
{
    Q_OBJECT
public:
    explicit OverlayWidget(QWidget *parent = nullptr);
    QSize containerSize();

signals:

public slots:
    virtual void setContainerSize(QSize container);

protected:
    // called whenever container rectangle changes
    virtual void recalculateGeometry() = 0;
    void paintEvent(QPaintEvent *event);

private:
    // size of whatever widget we are overlayed on
    QSize container;
};

#endif // OVERLAYWIDGET_H
