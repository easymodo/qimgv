#ifndef SLIDEPANEL_H
#define SLIDEPANEL_H

#include <QWidget>
#include <QTimeLine>
#include <QGraphicsScene>
#include <QGraphicsOpacityEffect>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QPainter>
#include <QGridLayout>
#include <QPushButton>
#include "settings.h"
#include <QDebug>

class SlidePanel : public QWidget {
    Q_OBJECT
public:
    explicit SlidePanel(QWidget *parent);
    ~SlidePanel();
    bool hasWidget();
    void setWidget(QWidget* w);
    virtual void containerResized(QSize parentSz) = 0;
    // useful if we want to change mouse hover area size
    virtual QSize triggerSize() = 0;

public slots:
    void show();

protected:
    QGridLayout mLayout;
    virtual void recalculateGeometry() = 0;
    QGraphicsOpacityEffect *fadeEffect;
    QPropertyAnimation *fadeAnimation, *slideAnimation;
    QParallelAnimationGroup *animGroup;
    QSize preferredWidgetSize;
    int panelSize;
    QSize parentSz;
    QWidget *mWidget;
    QPoint initialPosition;
    void leaveEvent(QEvent *event);
};

#endif // SLIDEPANEL_H
