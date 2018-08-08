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

    timeline.setDuration(230);
    timeline.setCurveShape(QTimeLine::LinearCurve);
    timeline.setStartFrame(0);
    timeline.setEndFrame(100);
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

// TODO: this misfires with QT_SCALE_FACTOR > 1.0
void SlidePanel::leaveEvent(QEvent *event) {
    Q_UNUSED(event)
    if(timeline.state() != QTimeLine::Running)
        timeline.start();
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
    // Calculate local cursor position; ocrrect for the current pos() animation
    QPoint adjustedPos = mapFromGlobal(QCursor::pos()) + this->pos();
    if(triggerRect().contains(adjustedPos, true)) {
        // Cancel the animation if cursor is back at the panel
        timeline.stop();
        fadeEffect->setOpacity(panelVisibleOpacity);
        setProperty("pos", startPosition);
    } else {
        // Apply the animation frame
        qreal value = outCurve.valueForProgress(frame / 100.0);
        QPoint newPos = QPoint(static_cast<int>(endPosition.x() * value),
                               static_cast<int>(endPosition.y() * value));
        setProperty("pos", newPos);
        fadeEffect->setOpacity(1 - value);
    }
}

void SlidePanel::onAnimationFinish() {
    QWidget::hide();
    fadeEffect->setOpacity(panelVisibleOpacity);
    setProperty("pos", startPosition);
}
