/*
 * Base class for auto-hiding panels.
 * Insert widget you want to show with setWidget().
 * Use setContainerSize() to notify panel of parent resize.
 */

#include "slidepanel.h"

SlidePanel::SlidePanel(QWidget *parent)
    : OverlayWidget(parent),
      panelSize(50),
      slideAmount(30),
      mWidget(NULL)
{
    mLayout.setSpacing(0);
    mLayout.setContentsMargins(0,0,0,0);
    this->setLayout(&mLayout);

    //fade & slide hover effect
    fadeEffect = new QGraphicsOpacityEffect(this);
    this->setGraphicsEffect(fadeEffect);
    fadeAnimation = new QPropertyAnimation(fadeEffect, "opacity");
    fadeAnimation->setDuration(230);
    fadeAnimation->setStartValue(1);
    fadeAnimation->setEndValue(0);
    fadeAnimation->setEasingCurve(QEasingCurve::OutQuart);
    slideAnimation = new QPropertyAnimation(this, "pos");
    slideAnimation->setDuration(300);
    slideAnimation->setStartValue(QPoint(0,0));
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
    return (mWidget != NULL);
}

void SlidePanel::leaveEvent(QEvent *event) {
    Q_UNUSED(event)
    // Workaround. For some reason this triggers at (0,0).
    if(QCursor::pos() == QPoint(0,0))
        return;
    animGroup->start(QPropertyAnimation::KeepWhenStopped);
}

void SlidePanel::show() {
    if(hasWidget()) {
        animGroup->stop();
        fadeEffect->setOpacity(1);
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
    fadeEffect->setOpacity(1);
    setProperty("pos", initialPosition);
}
