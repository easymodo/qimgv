#include "slidepanel.h"

SlidePanel::SlidePanel(QWidget *w, QWidget *parent)
    : QWidget(parent),
      panelSize(50),
      parentFullscreen(false),
      mWidget(NULL)
{
    parentSz = parent->size();
    layout = new QBoxLayout(QBoxLayout::TopToBottom);
    layout->setSpacing(0);
    layout->setContentsMargins(0,0,0,0);
    setWidget(w);
    this->setLayout(layout);
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
    slideAnimation->setEndValue(QPoint(0, -60));
    slideAnimation->setEasingCurve(QEasingCurve::OutQuart);

    animGroup = new QParallelAnimationGroup;
    animGroup->addAnimation(fadeAnimation);
    animGroup->addAnimation(slideAnimation);
    connect(animGroup, SIGNAL(finished()), this, SLOT(hide()), Qt::UniqueConnection);

    this->setAttribute(Qt::WA_NoMousePropagation, true);
    this->setFocusPolicy(Qt::NoFocus);
    this->hide();
}

SlidePanel::~SlidePanel() {

}

void SlidePanel::setPosition(PanelPosition p) {
    position = p;
    updatePanelPosition();
}

void SlidePanel::setWidget(QWidget *w) {
    if(!w) {
        qDebug() << "Error: SlidePanel::setWidget - null argument";
        return;
    }
    if(mWidget) {
        layout->removeWidget(mWidget);
    }
    mWidget = w;
    this->resize(mWidget->size());
    this->setGeometry(mWidget->geometry());
    mWidget->setParent(this);
    layout->addWidget(mWidget);
}

void SlidePanel::leaveEvent(QEvent *event) {
    Q_UNUSED(event)
    animGroup->start(QPropertyAnimation::KeepWhenStopped);
}

void SlidePanel::show() {
    animGroup->stop();
    fadeEffect->setOpacity(1);
    setProperty("pos", initialPosition);
    QWidget::show();
}
