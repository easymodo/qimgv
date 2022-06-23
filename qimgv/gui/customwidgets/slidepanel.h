#pragma once

#include <QtGlobal>
#include <QTimeLine>
#include <QGraphicsOpacityEffect>
#include <QPainter>
#include <QBoxLayout>
#include <QTimer>
#include <QTimeLine>
#include "floatingwidget.h"
#include "settings.h"
#include <memory>
#include <QDebug>
#include <ctime>

class SlidePanel : public FloatingWidget {
    Q_OBJECT
public:
    explicit SlidePanel(FloatingWidgetContainer *parent);
    ~SlidePanel();
    bool hasWidget();
    void setWidget(std::shared_ptr<QWidget> w);
    // Use visibleGeometry instead of geometry() here.
    // If this is called mid-animation then geometry() will be all wrong.
    QRect triggerRect();
    // when this is set, the widget will not change geometry by itself
    // no pos() animations & no recalculateGeometry()
    bool layoutManaged();
    void setLayoutManaged(bool mode);

    void setPosition(PanelPosition);
    PanelPosition position();
    void hideAnimated();

public slots:
    void show();
    void hide();

private slots:
    void onAnimationFinish();
    void animationUpdate(int frame);

protected:
    QHBoxLayout mLayout;
    QGraphicsOpacityEffect *fadeEffect;
    int panelSize, slideAmount;
    std::shared_ptr<QWidget> mWidget;
    QRect mTriggerRect;
    void setAnimationRange(QPoint start, QPoint end);
    void saveStaticGeometry(QRect geometry);
    QRect staticGeometry();
    QTimer timer;
    QTimeLine timeline;
    QEasingCurve outCurve;
    const int ANIMATION_DURATION = 230;
    PanelPosition mPosition;
    void recalculateGeometry();
    virtual void updateTriggerRect();

private:
    void setOrientation();
    Qt::Orientation mOrientation;
    QRect mStaticGeometry;
    qreal panelVisibleOpacity = 1.0;
    QPoint startPosition, endPosition;
    bool mLayoutManaged = false;
};
