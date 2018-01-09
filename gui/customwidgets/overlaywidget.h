#ifndef OVERLAYWIDGET_H
#define OVERLAYWIDGET_H

#include <QWidget>
#include <QStyleOption>
#include <QPainter>

class OverlayWidget : public QWidget
{
    Q_OBJECT
public:
    explicit OverlayWidget(QWidget *parent);
    QSize containerSize();

signals:

public slots:
    virtual void setContainerSize(QSize container);

protected:
    // called whenever container rectangle changes
    virtual void recalculateGeometry() = 0;
    void paintEvent(QPaintEvent *event);

private:
    QSize container;
};

#endif // OVERLAYWIDGET_H
