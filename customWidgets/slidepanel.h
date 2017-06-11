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
#include "overlaywidget.h"
#include "settings.h"
#include <QDebug>

class SlidePanel : public OverlayWidget {
    Q_OBJECT
public:
    explicit SlidePanel(QWidget *parent);
    ~SlidePanel();
    bool hasWidget();
    void setWidget(QWidget* w);
    // Use visibleGeometry instead of geometry() here.
    // If this is called mid-animation then geometry() will be all wrong.
    virtual QRect triggerRect() = 0;

public slots:
    void show();

private slots:
    void readSettings();

protected:
    QGridLayout mLayout;
    QGraphicsOpacityEffect *fadeEffect;
    QPropertyAnimation *fadeAnimation, *slideAnimation;
    QParallelAnimationGroup *animGroup;
    QSize preferredWidgetSize;
    int panelSize, slideAmount;
    QWidget *mWidget;
    QPoint initialPosition;
    QRect mTriggerRect;
    virtual void updateTriggerRect() = 0;
    void leaveEvent(QEvent *event);
    bool animated;
};

#endif // SLIDEPANEL_H
