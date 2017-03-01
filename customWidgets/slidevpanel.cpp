#include "slidevpanel.h"

SlideVPanel::SlideVPanel(QWidget *w, QWidget *parent)
    : QWidget(parent),
      panelSize(122),
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
    // TODO: bottom panel position
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

SlideVPanel::~SlideVPanel() {

}

void SlideVPanel::updatePanelPosition() {
    QRect oldRect = this->rect();
    qDebug() << mWidget->size();
    //if(position == TOP) {
        this->setGeometry(QRect(QPoint(0, 0),
                                QPoint(parentSz.width(), mWidget->height())));
    //} else {
   //     this->setGeometry(QRect(QPoint(0, parentSz.height() - mWidget->height() + 1),
   //                             QPoint(parentSz.width(), parentSz.height())));
   // }
    qDebug() << this->rect();
    if(oldRect != this->rect())
        emit panelSizeChanged();
}

// !move this to constructor
void SlideVPanel::setWidget(QWidget *w) {
    if(!w) {
        qDebug() << "Error: SlidePanel::setWidget - null argument";
        return;
    }
    if(mWidget) {
        layout->removeWidget(mWidget);
    }
    mWidget = w;
    this->setGeometry(mWidget->geometry());
    mWidget->setParent(this);
    layout->addWidget(mWidget);
}

void SlideVPanel::parentResized(QSize parentSz) {
    this->parentSz = parentSz;
    updatePanelPosition();
    mWidget->resize(this->width(), mWidget->height());
}

void SlideVPanel::show() {
    animGroup->stop();
    fadeEffect->setOpacity(1);
    setProperty("pos", QPoint(0,0));
    QWidget::show();
}

void SlideVPanel::leaveEvent(QEvent *event) {
    Q_UNUSED(event)
    animGroup->start(QPropertyAnimation::KeepWhenStopped);
}

void SlideVPanel::paintEvent(QPaintEvent *event) {
    QWidget::paintEvent(event);
    if(position == PanelPosition::TOP) {
        QPainter p(this);
        p.setPen(QColor(QColor(110, 110, 110)));
        p.drawLine(rect().bottomLeft(), rect().bottomRight());
    }
}
