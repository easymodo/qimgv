#ifndef SLIDEPANEL_H
#define SLIDEPANEL_H

#include <QtGlobal>
#include <QTimeLine>
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
    explicit SlidePanel(OverlayContainerWidget *parent);
    ~SlidePanel();
    bool hasWidget();
    void setWidget(std::shared_ptr<QWidget> w);
    // Use visibleGeometry instead of geometry() here.
    // If this is called mid-animation then geometry() will be all wrong.
    virtual QRect triggerRect() = 0;

public slots:
    void show();
    void hide();

private slots:
    void onAnimationFinish();

protected:
    QGridLayout mLayout;
    QGraphicsOpacityEffect *fadeEffect;
    QPropertyAnimation *fadeAnimation, *slideAnimation;
    QParallelAnimationGroup *animGroup;
    int panelSize, slideAmount;
    std::shared_ptr<QWidget> mWidget;
    QPoint initialPosition;
    QRect mTriggerRect;
    virtual void updateTriggerRect() = 0;
    void leaveEvent(QEvent *event);
    void saveStaticGeometry(QRect geometry);
    QRect staticGeometry();

private:
    QRect mStaticGeometry;
    qreal panelVisibleOpacity = 1.0f;

};

#endif // SLIDEPANEL_H
