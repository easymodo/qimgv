#ifndef SLIDEPANEL_H
#define SLIDEPANEL_H

#include <QtGlobal>
#include <QTimeLine>
#include <QGraphicsOpacityEffect>
#include <QPainter>
#include <QGridLayout>
#include <QTimer>
#include <QTimeLine>
#include "overlaywidget.h"
#include "settings.h"
#include <QDebug>
#include <ctime>

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

    void hideAnimated();
public slots:
    void show();
    void hide();

private slots:
    void onAnimationFinish();

    void animationUpdate(int frame);
protected:
    QGridLayout mLayout;
    QGraphicsOpacityEffect *fadeEffect;
    int panelSize, slideAmount;
    std::shared_ptr<QWidget> mWidget;
    QRect mTriggerRect;
    virtual void setAnimationRange(QPoint start, QPoint end);
    virtual void updateTriggerRect() = 0;
    void saveStaticGeometry(QRect geometry);
    QRect staticGeometry();

    QTimer timer;
    QTimeLine timeline;
    QEasingCurve outCurve;
    const int ANIMATION_DURATION = 230;

//    void mouseMoveEvent(QMouseEvent *event);
private:
    QRect mStaticGeometry;
    qreal panelVisibleOpacity = 1.0;
    QPoint startPosition, endPosition;

};

#endif // SLIDEPANEL_H
