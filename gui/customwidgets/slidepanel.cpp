/*
 *
 * Base class for auto-hiding panels.
 * Insert widget you want to show with setWidget().
 */

#include "slidepanel.h"

SlidePanel::SlidePanel(OverlayContainerWidget *parent)
    : OverlayWidget(parent),
      panelSize(50),
      slideAmount(40),
      mWidget(nullptr)
{
    // workaround for https://bugreports.qt.io/browse/QTBUG-66387
    if( strcmp(qVersion(), "5.10.1") == 0 || strcmp(qVersion(), "5.9.4") == 0) {
        panelVisibleOpacity = 0.999;
    }

    mLayout.setSpacing(0);
    mLayout.setContentsMargins(0,0,0,0);
    this->setLayout(&mLayout);

    //fade effect
    fadeEffect = new QGraphicsOpacityEffect(this);
    this->setGraphicsEffect(fadeEffect);

    startPosition = geometry().topLeft();

    outCurve.setType(QEasingCurve::OutQuart);

    timeline.setDuration(ANIMATION_DURATION);
    timeline.setCurveShape(QTimeLine::LinearCurve);
    timeline.setStartFrame(0);
    timeline.setEndFrame(ANIMATION_DURATION);
    // For some reason 16 feels janky on windows. Linux is fine.
#ifdef _WIN32
    timeline.setUpdateInterval(8);
#else
    timeline.setUpdateInterval(16);
#endif

    connect(&timeline, SIGNAL(frameChanged(int)), this, SLOT(animationUpdate(int)));
    connect(&timeline, SIGNAL(finished()), this, SLOT(onAnimationFinish()));

    this->setAttribute(Qt::WA_NoMousePropagation, true);
    this->setFocusPolicy(Qt::NoFocus);
    QWidget::hide();
}

SlidePanel::~SlidePanel() {
}

void SlidePanel::hide() {
    timeline.stop();
    QWidget::hide();
}

void SlidePanel::hideAnimated() {
    if(!this->isHidden() && timeline.state() != QTimeLine::Running) {
        timeline.start();
    }
}

void SlidePanel::setWidget(std::shared_ptr<QWidget> w) {
    if(!w)
        return;
    if(hasWidget())
        mLayout.removeWidget(mWidget.get());
    mWidget = w;
    mWidget->setParent(this);
    mLayout.addWidget(mWidget.get(), 0, 0);
}

bool SlidePanel::hasWidget() {
    return (mWidget != nullptr);
}

void SlidePanel::show() {
    if(hasWidget()) {
        timeline.stop();
        fadeEffect->setOpacity(panelVisibleOpacity);
        setProperty("pos", startPosition);
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

void SlidePanel::animationUpdate(int frame) {
    // Calculate local cursor position; correct for the current pos() animation
    QPoint adjustedPos = mapFromGlobal(QCursor::pos()) + this->pos();
    if(triggerRect().contains(adjustedPos, true)) {
        // Cancel the animation if cursor is back at the panel
        timeline.stop();
        fadeEffect->setOpacity(panelVisibleOpacity);
        setProperty("pos", startPosition);
    } else {
        // Apply the animation frame
        qreal value = outCurve.valueForProgress(static_cast<qreal>(frame) / ANIMATION_DURATION);
        QPoint newPosOffset = QPoint(static_cast<int>((endPosition.x() - startPosition.x()) * value),
                                     static_cast<int>((endPosition.y() - startPosition.y()) * value));
        setProperty("pos", startPosition + newPosOffset);
        fadeEffect->setOpacity(1 - value);
    }
    qApp->processEvents();
}

void SlidePanel::setAnimationRange(QPoint start, QPoint end) {
    startPosition = start;
    endPosition = end;
}

void SlidePanel::onAnimationFinish() {
    QWidget::hide();
    fadeEffect->setOpacity(panelVisibleOpacity);
    setProperty("pos", startPosition);
}
