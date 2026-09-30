#include "splitpane.h"

SplitPane::SplitPane(QWidget *content, QWidget *parent)
    : QWidget(parent),
      mFrameVisible(false),
      mActive(false)
{
    setAttribute(Qt::WA_TranslucentBackground, true);
    setMouseTracking(true);
    mLayout = new QBoxLayout(QBoxLayout::LeftToRight, this);
    mLayout->setContentsMargins(0, 0, 0, 0);
    mLayout->setSpacing(0);
    content->setParent(this);
    mLayout->addWidget(content);
}

void SplitPane::setFrameVisible(bool mode) {
    if(mFrameVisible == mode)
        return;
    mFrameVisible = mode;
    int margin = mode ? FRAME_WIDTH : 0;
    mLayout->setContentsMargins(margin, margin, margin, margin);
    update();
}

void SplitPane::setActive(bool mode) {
    if(mActive == mode)
        return;
    mActive = mode;
    update();
}

void SplitPane::paintEvent(QPaintEvent *event) {
    QWidget::paintEvent(event);
    if(!mFrameVisible)
        return;
    QPainter p(this);
    // straw yellow for the focused pane, grey for the other one
    QPen pen(mActive ? QColor(240, 226, 140) : QColor(105, 105, 105));
    pen.setWidth(FRAME_WIDTH);
    pen.setJoinStyle(Qt::MiterJoin);
    p.setPen(pen);
    qreal half = FRAME_WIDTH / 2.0;
    p.drawRect(QRectF(rect()).adjusted(half, half, -half, -half));
}
