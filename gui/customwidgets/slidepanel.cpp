/*
 *
 * Base class for auto-hiding panels.
 * Insert widget you want to show with setWidget().
 */

#include "slidepanel.h"

SlidePanel::SlidePanel(ContainerWidget *parent)
    : OverlayWidget(parent),
      panelSize(50),
      slideAmount(30),
      mWidget(nullptr)
{
    // workaround for https://bugreports.qt.io/browse/QTBUG-66387
    // TODO: remove this when it'll get fixed. And the QtGlobal include

    // ok i'm not sure when this will get fixed so let's apply this by default for now
    //if( strcmp(qVersion(), "5.10.1") == 0 || strcmp(qVersion(), "5.9.4") == 0) {
        panelVisibleOpacity = 0.999f;
    //}

    mLayout.setSpacing(0);
    mLayout.setContentsMargins(0,0,0,0);
    this->setLayout(&mLayout);

    //fade & slide hover effect
    fadeEffect = new QGraphicsOpacityEffect(this);
    this->setGraphicsEffect(fadeEffect);
    fadeAnimation = new QPropertyAnimation(fadeEffect, "opacity");
    fadeAnimation->setDuration(230);
    fadeAnimation->setStartValue(panelVisibleOpacity);
    fadeAnimation->setEndValue(0);
    fadeAnimation->setEasingCurve(QEasingCurve::OutQuart);
    slideAnimation = new QPropertyAnimation(this, "pos");
    slideAnimation->setDuration(300);
    slideAnimation->setStartValue(QPoint(0, 0));
    slideAnimation->setEndValue(QPoint(0, -slideAmount));
    slideAnimation->setEasingCurve(QEasingCurve::OutQuart);

    animGroup = new QParallelAnimationGroup;
    animGroup->addAnimation(fadeAnimation);
    animGroup->addAnimation(slideAnimation);
    connect(animGroup, SIGNAL(finished()),
            this, SLOT(onAnimationFinish()), Qt::UniqueConnection);

    this->setAttribute(Qt::WA_NoMousePropagation, true);
    this->setFocusPolicy(Qt::NoFocus);
    QWidget::hide();
}

SlidePanel::~SlidePanel() {
}

void SlidePanel::setWidget(QWidget *w) {
    if(!w)
        return;
    if(hasWidget())
        mLayout.removeWidget(mWidget);
    mWidget = w;
    mWidget->setParent(this);
    mLayout.addWidget(mWidget, 0, 0);
}

bool SlidePanel::hasWidget() {
    return (mWidget != nullptr);
}

// TODO: this misfires with QT_SCALE_FACTOR > 1.0
void SlidePanel::leaveEvent(QEvent *event) {
    Q_UNUSED(event)
    animGroup->start(QPropertyAnimation::KeepWhenStopped);
}

void SlidePanel::show() {
    if(hasWidget()) {
        animGroup->stop();
        fadeEffect->setOpacity(panelVisibleOpacity);
        setProperty("pos", initialPosition);
        QWidget::show();
    } else {
        qDebug() << "Warning: Trying to show panel containing no widget!";
    }
}

// save current geometry so it is accessible during "pos" animation
void SlidePanel::saveStaticGeometry(QRect geometry) {
    mStaticGeometry = geometry;
}

QRect SlidePanel::staticGeometry() {
    return mStaticGeometry;
}

void SlidePanel::onAnimationFinish() {
    QWidget::hide();
    fadeEffect->setOpacity(panelVisibleOpacity);
    setProperty("pos", initialPosition);
}
